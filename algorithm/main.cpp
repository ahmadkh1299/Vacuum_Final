#include "Simulation.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string housePath;
    std::string algoPath;
    int numThreads = 10;
    bool summaryOnly = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.rfind("-house_path=", 0) == 0) {
            housePath = arg.substr(12);
        } else if (arg.rfind("-algo_path=", 0) == 0) {
            algoPath = arg.substr(11);
        } else if (arg.rfind("-num_threads=", 0) == 0) {
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

    // Create and run simulation
    Simulation sim;
    sim.loadHouse(housePath);
    sim.loadAlgorithms(algoPath);
    sim.run();

    if (!summaryOnly) {
        sim.generateSummary();
    }

    return 0;
}
