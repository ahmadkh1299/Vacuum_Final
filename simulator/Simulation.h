#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>
#include <memory>
#include "House.h"
#include "AbstractAlgorithm.h"

class Simulation {
public:
    Simulation();
    ~Simulation() = default;
    void loadHouse(const std::string& housePath);
    void loadAlgorithms(const std::string& algoPath);
    void run();
    void generateSummary() const;

private:
    std::vector<std::unique_ptr<House>> houses;
    std::vector<std::unique_ptr<AbstractAlgorithm>> algorithms;
    int maxSteps;
    int numThreads;

    void runSimulation(House& house, AbstractAlgorithm& algorithm);
    void writeOutputFile(const std::string& houseName, const std::string& algoName,
                         int numSteps, int dirtLeft, bool finished, bool inDock, int score) const;
    int calculateScore(int numSteps, int dirtLeft, bool finished, bool inDock) const;
};

#endif // SIMULATION_H