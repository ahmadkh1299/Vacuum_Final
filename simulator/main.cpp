#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <dlfcn.h>
#include <fstream>
#include "../common/AlgorithmRegistrar.h"
#include "Simulation.h"

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

void loadAlgorithms(const std::string& algoPath, std::vector<void*>& handles) {
    for (auto& entry : fs::directory_iterator(algoPath)) {
        if (entry.path().extension() == ".so") {
            void* handle = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Error loading library " << entry.path() << ": " << dlerror() << std::endl;
                std::ofstream errorFile(entry.path().stem().string() + ".error");
                errorFile << "Failed to load algorithm: " << dlerror() << std::endl;
            } else {
                handles.push_back(handle);
            }
        }
    }
}

void cleanAlgorithms(std::vector<void*>& handles) {
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

    // Load algorithm libraries
    std::vector<void*> algoHandles;
    loadAlgorithms(algoPath, algoHandles);

    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    // Create and run simulation
    Simulation sim;
    sim.loadHouses(housePath);

    std::cout << "Running on " << registrar.count() << " algorithms." << std::endl;

    sim.runSimulations(registrar, numThreads, summaryOnly);

    if (!summaryOnly) {
        std::vector<std::string> houseFiles;
        for (const auto& entry : fs::directory_iterator(housePath)) {
            if (entry.path().extension() == ".house") {
                houseFiles.push_back(entry.path().string());
            }
        }
        sim.generateSummary(houseFiles, registrar);
    }

    // Clean up
    registrar.clear();
    cleanAlgorithms(algoHandles);

    return 0;
}