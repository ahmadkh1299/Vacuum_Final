cmake_minimum_required(VERSION 3.22)
project(Vacuum_Final)

set(CMAKE_CXX_STANDARD 14)

include_directories(algorithm)
include_directories(common)
include_directories(simulator)

add_executable(Vacuum_Final
        algorithm/AlgorithmDFS.cpp
        algorithm/AlgorithmDFS.h
        algorithm/AlgorithmRegistration.h
        algorithm/Algorithm_212346076_207177197_B.cpp
        algorithm/Algorithm_212346076_207177197_B.h
        common/AbstractAlgorithm.h
        common/AlgorithmRegistrar.h
        common/BatteryMeter.h
        common/DirtSensor.h
        common/enums.h
        common/WallSensor.h
        simulator/AlgorithmRegistrar.cpp
        simulator/ConfigReader.h
        simulator/House.cpp
        simulator/House.h
        simulator/Simulation.cpp
        simulator/Simulation.h
        simulator/Vacuum.cpp
        simulator/Vacuum.h
        main.cpp simulator/Explorer.cpp simulator/Explorer.h common/PositionUtils.cpp common/PositionUtils.h)
