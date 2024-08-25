#include "Simulation.h"
#include "ConfigReader.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <future>

namespace fs = std::filesystem;

Simulation::Simulation() {}

void Simulation::loadHouses(const std::string& housePath) {
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            try {
                ConfigReader config(entry.path().string());
                houses.push_back(std::make_unique<House>(config.getLayout(), config.getHouseName()));
                maxSteps.push_back(config.getMaxSteps());
                maxBatteries.push_back(config.getMaxBattery());
            } catch (const std::exception& e) {
                std::cerr << "Error loading house file " << entry.path() << ": " << e.what() << std::endl;
                std::ofstream errorFile(entry.path().stem().string() + ".error");
                errorFile << "Error loading house file: " << e.what() << std::endl;
            }
        }
    }
}

void Simulation::runSimulations(const AlgorithmRegistrar& registrar, int numThreads, bool summaryOnly) {
    std::vector<std::thread> threads;
    std::atomic<int> activeThreads(0);

    for (size_t i = 0; i < houses.size(); ++i) {
        for (const auto& algo : registrar) {
            while (activeThreads >= numThreads) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            threads.emplace_back([this, i, &algo, summaryOnly, &activeThreads]() {
                activeThreads++;
                runSimulation(*houses[i], algo.create(), algo.name(), maxSteps[i], maxBatteries[i], summaryOnly);
                activeThreads--;
            });
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void Simulation::runSimulation(const House& house, std::unique_ptr<AbstractAlgorithm> algo, const std::string& algoName,
                               int maxSteps, int maxBattery, bool summaryOnly) {
    int initialDirt = house.getTotalDirt();
    auto timeout = std::chrono::milliseconds(maxSteps);

    std::atomic<bool> finished(false);
    std::atomic<int> steps(0);
    std::atomic<int> dirtLeft(initialDirt);
    std::atomic<bool> inDock(true);

    auto futureResult = std::async(std::launch::async, [&]() {
        House simHouse = house; // Create a copy of the house for this simulation
        Vacuum vacuum;
        vacuum.init(maxBattery, simHouse.getDockingStation());
        SensorImpl sensor(simHouse, maxBattery);
        initializeAlgorithm(*algo, sensor, maxSteps);

        std::string stepsString;

        simulationLoop(simHouse, *algo, vacuum, sensor, steps, finished, dirtLeft, inDock, stepsString, summaryOnly, maxSteps);

        if (!summaryOnly) {
            writeOutputFile(simHouse.getName(), algoName, steps, dirtLeft, finished, inDock,
                            calculateScore(steps, dirtLeft, finished, inDock, maxSteps, initialDirt), stepsString);
        }
    });

    if (futureResult.wait_for(timeout) == std::future_status::timeout) {
        // Simulation timed out
        int score = maxSteps * 2 + initialDirt * 300 + 2000;
        {
            std::lock_guard<std::mutex> lock(scoresMutex);
            scores[{house.getName(), algoName}] = score;
        }
        if (!summaryOnly) {
            writeOutputFile(house.getName(), algoName, maxSteps, initialDirt, false, false, score, "");
        }
    } else {
        // Simulation completed within timeout
        int score = calculateScore(steps, dirtLeft, finished, inDock, maxSteps, initialDirt);
        {
            std::lock_guard<std::mutex> lock(scoresMutex);
            scores[{house.getName(), algoName}] = score;
        }
    }
}

void Simulation::simulationLoop(House& house, AbstractAlgorithm& algo, Vacuum& vacuum, SensorImpl& sensor,
                                std::atomic<int>& steps, std::atomic<bool>& finished, std::atomic<int>& dirtLeft,
                                std::atomic<bool>& inDock, std::string& stepsString, bool summaryOnly, int maxSteps) {
    while (steps < maxSteps && !finished) {
        // Get next step from the algorithm
        Step step = algo.nextStep();
        stepsString += stepToString(step);

        // Update simulation state
        updateSimulationState(house, vacuum, sensor, step, dirtLeft, inDock);

        // Check for finish conditions
        if (step == Step::Finish || (house.isHouseClean() && inDock)) {
            finished = true;
        }

        steps++;
    }
}

void Simulation::initializeAlgorithm(AbstractAlgorithm& algo, const SensorImpl& sensor, int maxSteps) {
    algo.setMaxSteps(maxSteps);
    algo.setWallsSensor(sensor);
    algo.setDirtSensor(sensor);
    algo.setBatteryMeter(sensor);
}

void Simulation::updateSimulationState(House& house, Vacuum& vacuum, SensorImpl& sensor, Step step, std::atomic<int>& dirtLeft, std::atomic<bool>& inDock) {
    // Update vacuum
    vacuum.step(step);

    // Update sensor position
    sensor.updatePosition(step);

    // Check if in docking station
    inDock = vacuum.atDockingStation();

    // Clean current position
    Position currentPos = vacuum.getPosition();
    if (house.getDirtLevel(currentPos) > 0) {
        house.cleanCell(currentPos);
        dirtLeft = house.getTotalDirt();
    }

    // Update battery in sensor
    if (!inDock) {
        sensor.useBattery();
    }
}

int Simulation::calculateScore(int steps, int dirtLeft, bool finished, bool inDock, int maxSteps, int initialDirt) {
    if (steps >= maxSteps) {
        return maxSteps * 2 + initialDirt * 300 + 2000;
    } else if (finished && !inDock) {
        return maxSteps + dirtLeft * 300 + 3000;
    } else {
        return steps + dirtLeft * 300 + (inDock ? 0 : 1000);
    }
}

void Simulation::writeOutputFile(const std::string& houseName, const std::string& algoName,
                                 int numSteps, int dirtLeft, bool finished, bool inDock, int score, const std::string& steps) {
    std::string filename = houseName + "-" + algoName + ".txt";
    std::ofstream outFile(filename);
    outFile << "NumSteps = " << numSteps << std::endl;
    outFile << "DirtLeft = " << dirtLeft << std::endl;
    outFile << "Status = " << (finished ? "FINISHED" : (numSteps >= 1000 ? "WORKING" : "DEAD")) << std::endl;
    outFile << "InDock = " << (inDock ? "TRUE" : "FALSE") << std::endl;
    outFile << "Score = " << score << std::endl;
    outFile << "Steps:\n" << steps << std::endl;
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

void Simulation::generateSummary(const std::vector<std::string>& houseFiles, const AlgorithmRegistrar& registrar) {
    std::ofstream summaryFile("summary.csv");
    summaryFile << "Algorithm";
    for (const auto& house : houses) {
        summaryFile << "," << house->getName();
    }
    summaryFile << std::endl;

    for (const auto& algo : registrar) {
        summaryFile << algo.name();
        for (const auto& house : houses) {
            auto it = scores.find({house->getName(), algo.name()});
            if (it != scores.end()) {
                summaryFile << "," << it->second;
            } else {
                summaryFile << ",N/A";
            }
        }
        summaryFile << std::endl;
    }
}