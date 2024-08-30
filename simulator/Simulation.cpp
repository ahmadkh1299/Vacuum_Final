#include "Simulation.h"

#include "SensorImpl.h"

#include "Vacuum.h"

#include <filesystem>

#include <fstream>

#include <iostream>

#include <thread>

#include <future>

#include <chrono>

#include <climits>

#include <fstream>

#include <vector>

#include <algorithm>



namespace fs = std::filesystem;



Simulation::Simulation(std::vector<std::unique_ptr<House>> houses, std::vector<int> maxSteps, std::vector<int> maxBatteries)

    : houses(std::move(houses)), maxSteps(std::move(maxSteps)), maxBatteries(std::move(maxBatteries)) {}



void Simulation::runSimulations(const std::vector<std::pair<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>>>& algorithms, int numThreads, bool summaryOnly) {

    std::vector<std::thread> threads;

    std::atomic<size_t> houseIndex(0);



    auto worker = [this, &algorithms, &houseIndex, summaryOnly]() {

        while (true) {

            size_t index = houseIndex.fetch_add(1);

            if (index >= houses.size()) break;



            for (const auto& [algoName, algoFactory] : algorithms) {

                runSingleSimulation(*houses[index], algoFactory(), algoName, 

                                    maxSteps[index], maxBatteries[index], summaryOnly);

            }

        }

    };



    for (int i = 0; i < numThreads; ++i) {

        threads.emplace_back(worker);

    }



    for (auto& thread : threads) {

        thread.join();

    }

}



void Simulation::runSingleSimulation(const House& house, std::unique_ptr<AbstractAlgorithm> algo, 

                                     const std::string& algoName, int maxSteps, int maxBattery, bool summaryOnly) {

    House simHouse = house; // Create a copy of the house for this simulation

    int initialDirt = simHouse.getTotalDirt();



    auto start = std::chrono::high_resolution_clock::now();

    auto result = simulateAlgorithm(simHouse, *algo, maxSteps, maxBattery);

    auto end = std::chrono::high_resolution_clock::now();



    std::chrono::duration<double, std::milli> elapsed = end - start;

    if (elapsed > std::chrono::milliseconds(maxSteps)) {

        result.steps = maxSteps;

        result.finished = false;

        result.dirtLeft = initialDirt;

        result.inDock = false;

    }



    result.score = calculateScore(result, maxSteps, initialDirt);



    {

        std::lock_guard<std::mutex> lock(scoresMutex);

        scores[{house.getName(), algoName}] = result.score;

    }



    if (!summaryOnly) {

        writeOutputFile(house.getName(), algoName, result);

    }

}



Simulation::SimulationResult Simulation::simulateAlgorithm(House& house, AbstractAlgorithm& algo, 

                                                           int maxSteps, int maxBattery) {

    SimulationResult result{};

    result.dirtLeft = house.getTotalDirt();

    result.inDock = true;



    Vacuum vacuum;

    vacuum.init(maxBattery, house.getDockingStation());

    SensorImpl sensor(house, maxBattery);



    algo.setMaxSteps(maxSteps);

    algo.setWallsSensor(sensor);

    algo.setDirtSensor(sensor);

    algo.setBatteryMeter(sensor);



    Step step;



    while (result.steps < maxSteps && !result.finished) {

        if(house.isHouseClean() && result.inDock) {

            result.finished = true;

            result.stepsString+= stepToString(Step::Finish);

            std::cout << "House is clean, simulation finished" << std::endl;

            break;

        }

        step = algo.nextStep();

        result.stepsString += stepToString(step);



        vacuum.step(step);

        sensor.updatePosition(step);

        result.inDock = vacuum.atDockingStation();



        Position currentPos = vacuum.getPosition();

        std::cout << "currentPos: " << currentPos.r << ", " << currentPos.c << std::endl;

        if (step == Step::Stay) {

            if (house.getDirtLevel(currentPos) > 0) {

                house.cleanCell(currentPos);

                sensor.useBattery();

                result.dirtLeft = house.getTotalDirt();

            }

            if (result.inDock) {

                sensor.chargeBattery();

                vacuum.setBattery(sensor.getBatteryState());

            }

        }

        if (step !=Step::Stay && !result.inDock) {

            sensor.useBattery();

        }



        if (step == Step::Finish) {

            result.finished = true;

        }



        result.steps++;

    }

    if(result.inDock && step == Step::Finish)

    {

        result.stepsString += stepToString(Step::Finish);

    }

    return result;

}



int Simulation::calculateScore(const SimulationResult& result, int maxSteps, int initialDirt) const {

    if (result.steps >= maxSteps) {

        return maxSteps * 2 + initialDirt * 300 + 2000;

    } else if (result.finished && !result.inDock) {

        return maxSteps + result.dirtLeft * 300 + 3000;

    } else {

        return result.steps + result.dirtLeft * 300 + (result.inDock ? 0 : 1000);

    }

}



void Simulation::writeOutputFile(const std::string& houseName, const std::string& algoName, 

                                 const SimulationResult& result) const {

    std::string filename = houseName + "-" + algoName + ".txt";

    std::ofstream outFile(filename);

    outFile << "NumSteps = " << result.steps << std::endl;

    outFile << "DirtLeft = " << result.dirtLeft << std::endl;

    outFile << "Status = " << (result.finished ? "FINISHED" : (result.steps >= 1000 ? "WORKING" : "DEAD")) << std::endl;

    outFile << "InDock = " << (result.inDock ? "TRUE" : "FALSE") << std::endl;

    outFile << "Score = " << result.score << std::endl;

    outFile << "Steps:\n" << result.stepsString << std::endl;

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

/*

void Simulation::generateSummary() const {

    std::cout << "Starting generateSummary()" << std::endl;

    

    try {

        std::ofstream summaryFile("summary.csv");

        if (!summaryFile.is_open()) {

            throw std::runtime_error("Failed to open summary.csv for writing");

        }



        std::cout << "Writing header" << std::endl;

        summaryFile << "Algorithm";

        for (const auto& house : houses) {

            if (house) {

                summaryFile << "," << house->getName();

            } else {

                std::cout << "Warning: Null house pointer encountered" << std::endl;

            }

        }

        summaryFile << std::endl;



        std::cout << "Number of houses: " << houses.size() << std::endl;

        std::cout << "Number of scores: " << scores.size() << std::endl;



        if (scores.empty()) {

            std::cout << "Warning: scores map is empty" << std::endl;

        }



        for (const auto& [key, value] : scores) {

            std::cout << "Algorithm: " << key.second << ", House: " << key.first << ", Score: " << value << std::endl;

        }



        std::cout << "Writing data for each algorithm" << std::endl;

        for (const auto& [key, _] : scores) {

            const auto& algoName = key.second;

            summaryFile << algoName;

            for (const auto& house : houses) {

                if (house) {

                    auto it = scores.find({house->getName(), algoName});

                    if (it != scores.end()) {

                        summaryFile << "," << it->second;

                    } else {

                        summaryFile << ",N/A";

                    }

                } else {

                    summaryFile << ",ERROR";

                }

            }

            summaryFile << std::endl;

        }



        summaryFile.close();

        std::cout << "Finished generateSummary()" << std::endl;

    } catch (const std::exception& e) {

        std::cerr << "Exception in generateSummary(): " << e.what() << std::endl;

    } catch (...) {

        std::cerr << "Unknown exception in generateSummary()" << std::endl;

    }

}*/

void Simulation::generateSummary() const {

    std::cout << "Starting generateSummary()" << std::endl;

    

    try {

        std::ofstream summaryFile("summary.csv");

        if (!summaryFile.is_open()) {

            throw std::runtime_error("Failed to open summary.csv for writing");

        }



        // Write header

        summaryFile << "Algorithm";

        for (const auto& house : houses) {

            if (house) {

                summaryFile << "," << house->getName();

            } else {

                summaryFile << ",Unknown House";

            }

        }

        summaryFile << std::endl;



        // Get unique algorithm names

        std::vector<std::string> uniqueAlgos;

        for (const auto& [key, _] : scores) {

            if (std::find(uniqueAlgos.begin(), uniqueAlgos.end(), key.second) == uniqueAlgos.end()) {

                uniqueAlgos.push_back(key.second);

            }

        }



        // Write data for each unique algorithm

        for (const auto& algoName : uniqueAlgos) {

            summaryFile << algoName;

            for (const auto& house : houses) {

                if (house) {

                    auto it = scores.find({house->getName(), algoName});

                    if (it != scores.end()) {

                        summaryFile << "," << it->second;

                    } else {

                        summaryFile << ",N/A";

                    }

                } else {

                    summaryFile << ",ERROR";

                }

            }

            summaryFile << std::endl;

        }



        summaryFile.close();

        std::cout << "Finished generateSummary()" << std::endl;

    } catch (const std::exception& e) {

        std::cerr << "Exception in generateSummary(): " << e.what() << std::endl;

    } catch (...) {

        std::cerr << "Unknown exception in generateSummary()" << std::endl;

    }

}
