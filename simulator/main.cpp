#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <dlfcn.h>
#include "AlgorithmRegistrar.h"
#include "Simulation.h"

namespace fs = std::filesystem;

void register_libs(const char* libs_dir, std::vector<void*>& libs) {
    for (const auto& entry : fs::directory_iterator(fs::path(libs_dir))) {
        if (entry.path().extension() == ".so") {
            void* lib_ptr = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (!lib_ptr) {
                std::cerr << "Error loading algorithm: " << dlerror() << std::endl;
                continue;
            }
            libs.push_back(lib_ptr);
        }
    }
}

void clear_libs(std::vector<void*>& libs) {
    for (void* lib : libs) {
        if (lib) {
            std::cout << "Closing library: " << lib << std::endl;
            if (dlclose(lib) != 0) {
                std::cerr << "Error closing library: " << dlerror() << std::endl;
            }
        }
    }
    std::cout << "Finished clearing libraries." << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <house_input_directory> <algorithm_directory>" << std::endl;
        return 1;
    }

    std::vector<std::string> houseFiles;
    std::vector<std::string> algorithmNames;
    std::vector<std::vector<int>> scores;
    std::vector<void*> algo_libs;

    std::string housePath = argv[1];
    std::string algoPath = argv[2];

    // Load house files
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            houseFiles.push_back(entry.path().string());
            std::cout << entry.path().string() << std::endl;
        }
    }

    // Register algorithm libraries
    register_libs(algoPath.c_str(), algo_libs);
    std::cout << "RUNNING ON : " << AlgorithmRegistrar::getAlgorithmRegistrar().count() << " algorithms." << std::endl;

    // Iterate through each registered algorithm
    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        std::cout << "Processing algorithm: " << algo.name() << std::endl;

        std::vector<int> currentScores;
        algorithmNames.push_back(algo.name());

        // Loop through each house file
        for (auto it = houseFiles.begin(); it != houseFiles.end(); ) {
            Simulation simulator;

            if (!simulator.readHouseFile(*it)) {
                std::cout << "Error reading the file " << *it << ". The file will be ignored." << std::endl;
                it = houseFiles.erase(it);
            } else {
                // Create the algorithm directly in the main function
                auto algorithm = algo.create();
                
                // Set the algorithm in the simulator
                simulator.setAlgorithm(algorithm.release(), algo.name());
                
                // Run the simulation and get the score
                int score = simulator.run();
                std::cout << "The score for " << algo.name() << " on " << *it << " is: " << score << std::endl;
                currentScores.push_back(score);

                ++it;
            }
        }

        // Store the scores for the current algorithm
        scores.push_back(currentScores);
    }

    // Write results to CSV file
    std::ofstream csvFile("summary.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not create summary.csv" << std::endl;
        return 1;
    }

    // Write the header
    csvFile << "Algorithm/House";
    for (const auto& house : houseFiles) {
        csvFile << "," << fs::path(house).stem().string();
    }
    csvFile << "\n";

    // Write the scores
    for (size_t i = 0; i < algorithmNames.size(); ++i) {
        csvFile << algorithmNames[i];
        for (const auto& score : scores[i]) {
            csvFile << "," << score;
        }
        csvFile << "\n";
    }

    csvFile.close();
    std::cout << "Summary written to summary.csv" << std::endl;

    // Clean up
    clear_libs(algo_libs);
    return 0;
}