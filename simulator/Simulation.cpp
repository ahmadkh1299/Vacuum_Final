#include "Simulation.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include "ConfigReader.h"
#include "AlgorithmRegistrar.h"

namespace fs = std::filesystem;

Simulation::Simulation() : maxSteps(0), numThreads(10) {}

void Simulation::loadHouse(const std::string& housePath) {
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            try {
                ConfigReader config(entry.path().string());
                houses.push_back(std::make_unique<House>(config.getLayout()));
                maxSteps = std::max(maxSteps, config.getMaxSteps());
            } catch (const std::exception& e) {
                std::cerr << "Error loading house file " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
}

void Simulation::loadAlgorithms(const std::string& algoPath) {
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for (const auto& entry : fs::directory_iterator(algoPath)) {
        if (entry.path().extension() == ".so") {
            // Load shared library and register algorithm
            // This part depends on your specific implementation of dynamic loading
            // You might need to use dlopen, dlsym, etc.
        }
    }

    for (const auto& algo : registrar) {
        algorithms.push_back(algo.create());
    }
}

void Simulation::run() {
    std::vector<std::thread> threads;
    std::mutex outputMutex;

    for (const auto& house : houses) {
        for (const auto& algo : algorithms) {
            threads.emplace_back([this, &house, &algo, &outputMutex]() {
                runSimulation(*house, *algo);
            });

            if (threads.size() >= numThreads) {
                for (auto& t : threads) {
                    t.join();
                }
                threads.clear();
            }
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

void Simulation::runSimulation(House& house, AbstractAlgorithm& algorithm) {
    int steps = 0;
    int battery = maxSteps; // Assuming maxSteps is also max battery
    int currentRow = house.getDockingStationRow();
    int currentCol = house.getDockingStationCol();
    bool finished = false;

    algorithm.setMaxSteps(maxSteps);
    // Set other sensors...

    while (steps < maxSteps && !finished && battery > 0) {
        Step step = algorithm.nextStep();

        if (step == Step::Finish) {
            finished = true;
            break;
        }

        // Update position based on step
        // Update battery
        // Clean current cell if dirty
        // Update steps

        if (house.isInDock(currentRow, currentCol)) {
            battery = std::min(battery + maxSteps / 20, maxSteps);
        }
    }

    bool inDock = house.isInDock(currentRow, currentCol);
    int dirtLeft = house.getTotalDirt();
    int score = calculateScore(steps, dirtLeft, finished, inDock);

    writeOutputFile(house.getName(), algorithm.getName(), steps, dirtLeft, finished, inDock, score);
}

void Simulation::writeOutputFile(const std::string& houseName, const std::string& algoName,
                                 int numSteps, int dirtLeft, bool finished, bool inDock, int score) const {
    std::string filename = houseName + "-" + algoName + ".txt";
    std::ofstream outFile(filename);

    outFile << "NumSteps = " << numSteps << std::endl;
    outFile << "DirtLeft = " << dirtLeft << std::endl;
    outFile << "Status = " << (finished ? "FINISHED" : (numSteps >= maxSteps ? "WORKING" : "DEAD")) << std::endl;
    outFile << "InDock = " << (inDock ? "TRUE" : "FALSE") << std::endl;
    outFile << "Score = " << score << std::endl;
    // Write steps...
}

int Simulation::calculateScore(int numSteps, int dirtLeft, bool finished, bool inDock) const {
    if (numSteps >= maxSteps) {
        return maxSteps + dirtLeft * 300 + (finished && !inDock ? 3000 : 0);
    } else {
        return numSteps + dirtLeft * 300 + (!inDock ? 1000 : 0);
    }
}

void Simulation::generateSummary() const {
    std::ofstream summaryFile("summary.csv");
    // Generate CSV with scores for each house-algorithm pair
}