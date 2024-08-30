#include "../common/AlgorithmRegistrar.h"
#include <iostream>
#include <stdexcept>

AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() { return registrar; }

void AlgorithmRegistrar::registerAlgorithm(const std::string& name, AlgorithmFactory algorithmFactory) {
    std::cout << "Registering algorithm: " << name << std::endl;
    algorithms.emplace_back(name, std::move(algorithmFactory));
    std::cout << "Algorithm registered successfully: " << name << std::endl;
}

// You might want to add implementations for other methods here if needed