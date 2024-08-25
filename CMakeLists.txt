cmake_minimum_required(VERSION 3.13)
project(vacuum_simulator VERSION 1.0 LANGUAGES CXX)

# Set C++20 for all targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define include directories
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/algorithm
    ${CMAKE_SOURCE_DIR}/simulator
)

# Add your first algorithm library
add_library(Algorithm_212346076_207177197_B SHARED
    algorithm/Algorithm_212346076_207177197_B.cpp
    simulator/Explorer.cpp
    simulator/House.cpp
    simulator/AlgorithmRegistrar.cpp  # <-- Correct path
    common/PositionUtils.cpp
    common/SensorImpl.cpp
)
target_include_directories(Algorithm_212346076_207177197_B PUBLIC ${INCLUDE_DIRS})

# Add your second algorithm library
add_library(AlgorithmDFS SHARED
    algorithm/AlgorithmDFS.cpp
    simulator/Explorer.cpp
    simulator/House.cpp
    simulator/AlgorithmRegistrar.cpp  # <-- Correct path
    common/PositionUtils.cpp
    common/SensorImpl.cpp
)
target_include_directories(AlgorithmDFS PUBLIC ${INCLUDE_DIRS})

# Add the main executable
add_executable(main
    simulator/main.cpp
    simulator/AlgorithmRegistrar.cpp  # <-- Correct path
    simulator/Simulation.cpp
    simulator/House.cpp
    simulator/Vacuum.cpp
    simulator/Explorer.cpp
    common/PositionUtils.cpp
    common/SensorImpl.cpp
)

# Set include directories for the main executable
target_include_directories(main PUBLIC ${INCLUDE_DIRS})

# Link against the necessary libraries
if (WIN32)
    target_link_libraries(main Algorithm_212346076_207177197_B AlgorithmDFS)
else()
    target_link_libraries(main dl Algorithm_212346076_207177197_B AlgorithmDFS)
endif()

# Add the -rdynamic flag to the linker options if needed (for Linux/Unix)
if (UNIX AND NOT APPLE)
    target_link_options(main PUBLIC "-rdynamic")
endif()
