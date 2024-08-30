#include "../common/AlgorithmRegistrar.h"

AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() { return registrar; }
extern "C" {
    AlgorithmRegistrar& getAlgorithmRegistrar() {
        return AlgorithmRegistrar::getAlgorithmRegistrar();
    }
}