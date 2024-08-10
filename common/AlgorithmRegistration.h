//
// Created by Mariam on 8/10/2024.
//
/*
#ifndef VACUUM_FINAL_ALGORITHMREGISTRATION_H
#define VACUUM_FINAL_ALGORITHMREGISTRATION_H
#pragma once

#include "../Common/AlgorithmRegistrar.h"
#include <string>

struct AlgorithmRegistration {
    AlgorithmRegistration(const std::string &name,
                          AlgorithmFactory algorithmFactory) {
        AlgorithmRegistrar::getAlgorithmRegistrar().registerAlgorithm(
                name, std::move(algorithmFactory));
    }
};

#define REGISTER_ALGORITHM(ALGO)                                               \
  AlgorithmRegistration _##ALGO(#ALGO, [] { return std::make_unique<ALGO>(); })

#endif //VACUUM_FINAL_ALGORITHMREGISTRATION_H
*/
#ifndef ALGORITHM_REGISTRATION_H
#define ALGORITHM_REGISTRATION_H

#include "AlgorithmRegistrar.h"

struct AlgorithmRegistration {
    AlgorithmRegistration(const std::string& name, AlgorithmFactory algorithmFactory) {
        AlgorithmRegistrar::getAlgorithmRegistrar().registerAlgorithm(name, std::move(algorithmFactory));
    }
};

#define REGISTER_ALGORITHM(ALGO) AlgorithmRegistration _##ALGO(#ALGO, []{return std::make_unique<ALGO>();})

#endif // ALGORITHM_REGISTRATION_H