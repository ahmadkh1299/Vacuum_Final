#include "Simulation.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <dlfcn.h>
#include "ConfigReader.h"
#include "AlgorithmRegistrar.h"
#include "Explorer.h"
#include "SensorImpl.h"

namespace fs = std::filesystem;

Simulation::Simulation() : maxSteps(0), numThreads(10) {}

void Simulation::loadHouse(const std::string& housePath) {
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            try {
                ConfigReader config(entry.path().string());
                houses.push_back(std::make_unique<House>(config.getLayout()));
                maxSteps = std::max(maxSteps, config.getMaxSteps());
                maxBattery = config.getMaxBattery();
            } catch (const std::exception& e) {
                std::ofstream errorFile(entry.path().stem().string() + ".error");
                errorFile << "Error loading house file " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
}

void Simulation::loadAlgorithms(const std::string& algoPath) {
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for (const auto& entry : fs::directory_iterator(algoPath)) {
        if (entry.path().extension() == ".so") {
            try {
                // Load shared library and register algorithm
                void* handle = dlopen(entry.path().c_str(), RTLD_NOW);
                if (!handle) {
                    throw std::runtime_error(dlerror());
                }

                dlclose(handle);
            } catch (const std::exception& e) {
                std::ofstream errorFile(entry.path().stem().string() + ".error");
                errorFile << "Error loading algorithm file " << entry.path() << ": " << e.what() << std::endl;
            }
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
                std::condition_variable cv;
                std::mutex cv_m;
                bool finished = false;
                std::thread t([&]() {
                    runSimulation(*house, *algo);
                    finished = true;
                    cv.notify_one();
                });

                std::unique_lock<std::mutex> lk(cv_m);
                if(cv.wait_for(lk, std::chrono::milliseconds(maxSteps), [&]{ return finished; })) {
                    t.join();
                } else {
                    // Handle timeout
                    // ...
                    t.detach();
                }
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
    Vacuum vacuum = Vacuum();
    vacuum.init(maxBattery, house.getDockingStation(), house.getDockingStation());

    SensorImpl sensors(house, maxSteps);

    algorithm.setWallsSensor(sensors);
    algorithm.setDirtSensor(sensors);
    algorithm.setBatteryMeter(sensors);
    algorithm.setMaxSteps(maxSteps);

    int stepsTaken = 0;
    bool finished = false;

    int final_score = calculateScore(stepsTaken, house.getTotalDirt(), finished, vacuum.atDockingStation());
    writeOutputFile(house.getName(), algorithm.getName(), stepsTaken, house.getTotalDirt(), finished, vacuum.atDockingStation(), final_score, sensors.getSteps());
}

void Simulation::writeOutputFile(const std::string& houseName, const std::string& algoName,
                                 int numSteps, int dirtLeft, bool finished, bool inDock, int score, const std::string& steps) const {
    std::string filename = houseName + "-" + algoName + ".txt";
    std::ofstream outFile(filename);

    outFile << "NumSteps = " << numSteps << std::endl;
    outFile << "DirtLeft = " << dirtLeft << std::endl;
    outFile << "Status = " << (finished ? "FINISHED" : (numSteps >= maxSteps ? "WORKING" : "DEAD")) << std::endl;
    outFile << "InDock = " << (inDock ? "TRUE" : "FALSE") << std::endl;
    outFile << "Score = " << score << std::endl;
    outFile << "Steps:\n" << steps << std::endl;
}

int Simulation::calculateScore(int numSteps, int dirtLeft, bool finished, bool inDock) const {
    if (numSteps >= maxSteps) {
        return maxSteps + dirtLeft * 300 + 2000;
    } else if (finished && !inDock) {
        return maxSteps + dirtLeft * 300 + 3000;
    } else {
        return numSteps + dirtLeft * 300 + (inDock ? 0 : 1000);
    }
}

void Simulation::generateSummary() const {
    std::ofstream summaryFile("summary.csv");
    // Generate CSV with scores for each house-algorithm pair
}