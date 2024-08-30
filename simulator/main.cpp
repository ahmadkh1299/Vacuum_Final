#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <dlfcn.h>
#include <fstream>
#include <functional>
#include "Simulation.h"
#include "ConfigReader.h"
#include "AlgorithmRegistrar.h"

namespace fs = std::filesystem;

std::string getArgValue(int argc, char* argv[], const std::string& arg) {
    for (int i = 1; i < argc; ++i) {
        std::string argStr = argv[i];
        if (argStr.find(arg) == 0) {
            return argStr.substr(arg.length());
        }
    }
    return "";
}

void loadHouses(const std::string& housePath, std::vector<std::unique_ptr<House>>& houses, 
                std::vector<int>& maxSteps, std::vector<int>& maxBatteries) {
    std::cout << "Loading houses from: " << housePath << std::endl;
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            try {
                ConfigReader config(entry.path().string());
                houses.push_back(std::make_unique<House>(config.getLayout(), config.getHouseName()));
                maxSteps.push_back(config.getMaxSteps());
                maxBatteries.push_back(config.getMaxBattery());
                std::cout << "Loaded house: " << config.getHouseName() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error loading house file " << entry.path() << ": " << e.what() << std::endl;
                std::ofstream errorFile(entry.path().stem().string() + ".error");
                errorFile << "Error loading house file: " << e.what() << std::endl;
            }
        }
    }
    std::cout << "Total houses loaded: " << houses.size() << std::endl;
}

void loadAlgorithms(const std::string& algoPath, std::vector<void*>& handles, 
                    std::vector<std::pair<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>>>& algorithms) {
    std::cout << "Loading algorithms from: " << algoPath << std::endl;
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    
    for (const auto& entry : fs::directory_iterator(algoPath)) {
        if (entry.path().extension() == ".so") {
            std::cout << "Attempting to load: " << entry.path() << std::endl;
            void* handle = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Error loading library " << entry.path() << ": " << dlerror() << std::endl;
                std::ofstream errorFile(entry.path().stem().string() + ".error");
                errorFile << "Failed to load algorithm: " << dlerror() << std::endl;
            } else {
                handles.push_back(handle);
                std::cout << "Successfully loaded: " << entry.path() << std::endl;
            }
        }
    }

    std::cout << "Checking registered algorithms..." << std::endl;
    for (const auto& algo : registrar) {
        algorithms.emplace_back(algo.name(), [&algo]() { return algo.create(); });
        std::cout << "Registered algorithm: " << algo.name() << std::endl;
    }
    std::cout << "Total algorithms registered: " << algorithms.size() << std::endl;
}

void cleanupAlgorithms(std::vector<void*>& handles) {
    for (auto& handle : handles) {
        dlclose(handle);
    }
}

int main(int argc, char* argv[]) {
    std::string housePath = getArgValue(argc, argv, "-house_path=");
    std::string algoPath = getArgValue(argc, argv, "-algo_path=");
    int numThreads = 10;
    bool summaryOnly = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.rfind("-num_threads=", 0) == 0) {
            try {
                numThreads = std::stoi(arg.substr(13));
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid value for -num_threads. It must be a positive integer." << std::endl;
                return 1;
            }
        } else if (arg == "-summary_only") {
            summaryOnly = true;
        }
    }

    // Check that required arguments are provided
    if (housePath.empty() || algoPath.empty()) {
        std::cerr << "Error: Both -house_path and -algo_path must be provided." << std::endl;
        return 1;
    }

    // Validate number of threads
    if (numThreads <= 0) {
        std::cerr << "Error: -num_threads must be a positive integer." << std::endl;
        return 1;
    }

    // Log the settings being used
    std::cout << "Running simulation with the following settings:" << std::endl;
    std::cout << "House path: " << housePath << std::endl;
    std::cout << "Algorithm path: " << algoPath << std::endl;
    std::cout << "Number of threads: " << numThreads << std::endl;
    std::cout << "Summary only: " << (summaryOnly ? "Yes" : "No") << std::endl;

    // Load houses
 std::vector<std::unique_ptr<House>> houses;
    std::vector<int> maxSteps;
    std::vector<int> maxBatteries;
    loadHouses(housePath, houses, maxSteps, maxBatteries);

    // Load algorithms
    
    std::vector<void*> algoHandles;
    std::vector<std::pair<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>>> algorithms;
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    std::cout << "Contents of algorithm directory:" << std::endl;for (const auto& entry : std::filesystem::directory_iterator(algoPath)) {
    std::cout << entry.path() << std::endl;
}
    loadAlgorithms(algoPath, algoHandles, algorithms);

    if (houses.empty() || algorithms.empty()) {
        std::cerr << "Error: No houses or algorithms loaded. Exiting." << std::endl;
        cleanupAlgorithms(algoHandles);
        return 1;
    }

    // Create and run simulation
    std::cout << "Creating simulation..." << std::endl;
    Simulation sim(std::move(houses), std::move(maxSteps), std::move(maxBatteries));
    std::cout << "Running simulations..." << std::endl;
    sim.runSimulations(algorithms, numThreads, summaryOnly);
    

    if (!summaryOnly) {
        std::cout << "Generating summary..." << std::endl;
        sim.generateSummary();
    }
    std::cout << "Summary generated. Beginning cleanup..." << std::endl;


    // Cleanup
    cleanupAlgorithms(algoHandles);

    std::cout << "Cleanup completed. Exiting program." << std::endl;

    return 0;
}