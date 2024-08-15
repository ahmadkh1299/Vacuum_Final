#include "Simulation.h"
#include <iostream>

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
            numThreads = std::stoi(arg.substr(13));
        } else if (arg == "-summary_only") {
            summaryOnly = true;
        }
    }

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