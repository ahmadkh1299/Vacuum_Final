#include "Simulation.h"
#include "ConfigReader.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <pthread.h>

namespace fs = std::filesystem;

Simulation::Simulation(int numThreads) 
    : numThreads(numThreads), work(std::make_unique<boost::asio::io_context::work>(ioContext)) {
    for (int i = 0; i < numThreads; ++i) {
        threadPool.emplace_back([this] { ioContext.run(); });
    }
}

Simulation::~Simulation() {
    work.reset();
    ioContext.stop();
    for (auto& thread : threadPool) {
        thread.join();
    }
}

void Simulation::loadHouses(const std::string& housePath) {
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            try {
                ConfigReader config(entry.path().string());
                tasks.emplace_back(SimulationTask{
                    std::make_unique<House>(config.getLayout(), config.getHouseName()),
                    nullptr,
                    "",
                    config.getMaxSteps(),
                    0,
                    false
                });
            } catch (const std::exception& e) {
                std::cerr << "Error loading house file " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
}

void Simulation::runSimulations(const AlgorithmRegistrar& registrar) {
    taskCount = tasks.size() * registrar.count();

    for (auto& task : tasks) {
        for (const auto& algo : registrar) {
            task.algorithm = algo.create();
            task.algoName = algo.name();
            boost::asio::post(ioContext, [this, &task] { runSimulationTask(task); });
        }
    }

    // Wait for all tasks to complete
    std::unique_lock<std::mutex> lock(countMutex);
    countCV.wait(lock, [this] { return taskCount == 0; });
}

void Simulation::runSimulationTask(SimulationTask& task) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

    boost::asio::steady_timer timer(ioContext, std::chrono::milliseconds(task.maxSteps));
    auto start = std::chrono::system_clock::now();
    auto threadHandler = pthread_self();

    timer.async_wait([this, &task, start, threadHandler](const boost::system::error_code& ec) {
        this->timerHandler(ec, task, start, threadHandler);
    });

    // Run the simulation
    task.algorithm->setMaxSteps(task.maxSteps);
    // Set up sensors and run the algorithm...

    int steps = 0;
    int dirtLeft = task.house->getTotalDirt();
    bool inDock = true;
    std::string stepsString;

    while (steps < task.maxSteps && !task.finished) {
        Step step = task.algorithm->nextStep();
        stepsString += stepToString(step);

        // Process the step, update house state, etc.
        // Update dirtLeft and inDock based on the step

        if (step == Step::Finish) {
            task.finished = true;
        }
        steps++;
    }

    timer.cancel();

    if (!task.finished.exchange(true)) {
        int score = calculateScore(steps, dirtLeft, task.finished, inDock, task.maxSteps);
        task.score = score;
        writeOutputFile(task.house->getName(), task.algoName, steps, dirtLeft, task.finished, inDock, score, stepsString);
    }

    // Decrement task count and notify
    {
        std::lock_guard<std::mutex> lock(countMutex);
        taskCount--;
    }
    countCV.notify_one();
}

void Simulation::timerHandler(const boost::system::error_code& ec, SimulationTask& task, 
                              std::chrono::system_clock::time_point start, pthread_t threadHandler) {
    if (ec == boost::asio::error::operation_aborted) {
        std::cout << "Timer for task " << task.house->getName() << " - " << task.algoName << " was canceled" << std::endl;
    } else if (!ec) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        std::cout << "Timer for task " << task.house->getName() << " - " << task.algoName 
                  << " expired after " << duration.count() << "ms" << std::endl;

        if (!task.finished.exchange(true)) {
            int score = task.maxSteps * 2 + task.house->getTotalDirt() * 300 + 2000;
            task.score = score;
            writeOutputFile(task.house->getName(), task.algoName, task.maxSteps, task.house->getTotalDirt(), 
                            false, false, score, "");
            pthread_cancel(threadHandler);
        }

        // Decrement task count and notify
        {
            std::lock_guard<std::mutex> lock(countMutex);
            taskCount--;
        }
        countCV.notify_one();
    }
}

void Simulation::generateSummary() const {
    std::ofstream summaryFile("summary.csv");
    if (!summaryFile.is_open()) {
        std::cerr << "Error: Could not create summary.csv" << std::endl;
        return;
    }

    summaryFile << "Algorithm/House";
    for (const auto& task : tasks) {
        summaryFile << "," << task.house->getName();
    }
    summaryFile << "\n";

    std::lock_guard<std::mutex> lock(scoreMutex);
    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        summaryFile << algo.name();
        for (const auto& task : tasks) {
            auto it = std::find_if(tasks.begin(), tasks.end(), [&](const SimulationTask& t) {
                return t.house->getName() == task.house->getName() && t.algoName == algo.name();
            });
            if (it != tasks.end()) {
                summaryFile << "," << it->score.load();
            } else {
                summaryFile << ",N/A";
            }
        }
        summaryFile << "\n";
    }

    summaryFile.close();
    std::cout << "Summary written to summary.csv" << std::endl;
}

void Simulation::writeOutputFile(const std::string& houseName, const std::string& algoName,
                                 int numSteps, int dirtLeft, bool finished, bool inDock, 
                                 int score, const std::string& steps) const {
    std::string filename = houseName + "-" + algoName + ".txt";
    std::ofstream outFile(filename);
    outFile << "NumSteps = " << numSteps << std::endl;
    outFile << "DirtLeft = " << dirtLeft << std::endl;
    outFile << "Status = " << (finished ? "FINISHED" : (numSteps >= 1000 ? "WORKING" : "DEAD")) << std::endl;
    outFile << "InDock = " << (inDock ? "TRUE" : "FALSE") << std::endl;
    outFile << "Score = " << score << std::endl;
    outFile << "Steps:\n" << steps << std::endl;
}

int Simulation::calculateScore(int steps, int dirtLeft, bool finished, bool inDock, int maxSteps) const {
    if (steps >= maxSteps) {
        return maxSteps * 2 + dirtLeft * 300 + 2000;
    } else if (finished && !inDock) {
        return maxSteps + dirtLeft * 300 + 3000;
    } else {
        return steps + dirtLeft * 300 + (inDock ? 0 : 1000);
    }
}

std::string Simulation::stepToString(Step step) {
    switch (step) {
        case Step::North: return "N";
        case Step::South: return "S";
        case Step::East: return "E";
        case Step::West: return "W";
        case Step::Stay: return "s";
        case Step::Finish: return "F";
        default: return "?";
    }
}