#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <map>
#include <functional>  // Add this include for std::function
#include "House.h"
#include "AbstractAlgorithm.h"

class Simulation {
public:
    Simulation(std::vector<std::unique_ptr<House>> houses, std::vector<int> maxSteps, std::vector<int> maxBatteries);
    ~Simulation() = default;

    void runSimulations(const std::vector<std::pair<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>>>& algorithms, int numThreads, bool summaryOnly);
    void generateSummary() const;

private:
    struct SimulationResult {
        int steps;
        int dirtLeft;
        bool finished;
        bool inDock;
        int score;
        std::string stepsString;
    };

    std::vector<std::unique_ptr<House>> houses;
    std::vector<int> maxSteps;
    std::vector<int> maxBatteries;
    std::map<std::pair<std::string, std::string>, int> scores; // (houseName, algoName) -> score
    std::mutex scoresMutex;

    void runSingleSimulation(const House& house, std::unique_ptr<AbstractAlgorithm> algo, 
                             const std::string& algoName, int maxSteps, int maxBattery, bool summaryOnly);
    SimulationResult simulateAlgorithm(House& house, AbstractAlgorithm& algo, int maxSteps, int maxBattery);
    int calculateScore(const SimulationResult& result, int maxSteps, int initialDirt) const;
    void writeOutputFile(const std::string& houseName, const std::string& algoName, const SimulationResult& result) const;
    static std::string stepToString(Step step);
};