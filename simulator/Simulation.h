#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <boost/asio.hpp>
#include "House.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistrar.h"

class Simulation {
public:
    Simulation(int numThreads);
    ~Simulation();

    void loadHouses(const std::string& housePath);
    void runSimulations(const AlgorithmRegistrar& registrar);
    void generateSummary() const;

private:
    struct SimulationTask {
        std::unique_ptr<House> house;
        std::unique_ptr<AbstractAlgorithm> algorithm;
        std::string algoName;
        int maxSteps;
        std::atomic<int> score;
        std::atomic<bool> finished;
    };

    void runSimulationTask(SimulationTask& task);
    void timerHandler(const boost::system::error_code& ec, SimulationTask& task, 
                      std::chrono::system_clock::time_point start, pthread_t threadHandler);

    int numThreads;
    std::vector<SimulationTask> tasks;
    mutable std::mutex scoreMutex;
    boost::asio::io_context ioContext;
    std::unique_ptr<boost::asio::io_context::work> work;
    std::vector<std::thread> threadPool;
    
    std::mutex countMutex;
    std::condition_variable countCV;
    std::atomic<int> taskCount{0};

    void writeOutputFile(const std::string& houseName, const std::string& algoName,
                         int numSteps, int dirtLeft, bool finished, bool inDock, int score, const std::string& steps) const;
    int calculateScore(int steps, int dirtLeft, bool finished, bool inDock, int maxSteps) const;
    static std::string stepToString(Step step);
};