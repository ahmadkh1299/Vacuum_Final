// Simulation.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <map>
#include "House.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistrar.h"
#include "Vacuum.h"
#include "SensorImpl.h"

class Simulation {
public:
    Simulation();
    ~Simulation() = default;

    void loadHouses(const std::string& housePath);
    void runSimulations(const AlgorithmRegistrar& registrar, int numThreads, bool summaryOnly);
    void generateSummary(const std::vector<std::string>& houseFiles, const AlgorithmRegistrar& registrar);

private:
    std::vector<std::unique_ptr<House>> houses;
    std::vector<int> maxSteps;
    std::vector<int> maxBatteries;
    std::map<std::pair<std::string, std::string>, int> scores; // (houseName, algoName) -> score
    std::mutex scoresMutex;

    void runSimulation(const House& house, std::unique_ptr<AbstractAlgorithm> algo, const std::string& algoName, bool summaryOnly);
    void simulationLoop(House& house, AbstractAlgorithm& algo, Vacuum& vacuum, SensorImpl& sensor,
                        std::atomic<int>& steps, std::atomic<bool>& finished, std::atomic<int>& dirtLeft,
                        std::atomic<bool>& inDock, std::string& stepsString, bool summaryOnly, int maxSteps);
    void initializeAlgorithm(AbstractAlgorithm& algo, const SensorImpl& sensor, int maxSteps);
    void updateSimulationState(House& house, Vacuum& vacuum, SensorImpl& sensor, Step step, std::atomic<int>& dirtLeft, std::atomic<bool>& inDock);
    int calculateScore(int steps, int dirtLeft, bool finished, bool inDock, int maxSteps, int initialDirt);
    void writeOutputFile(const std::string& houseName, const std::string& algoName,
                         int numSteps, int dirtLeft, bool finished, bool inDock, int score, const std::string& steps);
    std::string stepToString(Step step);
};