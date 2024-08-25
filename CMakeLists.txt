cmake_minimum_required(VERSION 3.13)
project(vacuum_simulator VERSION 1.0 LANGUAGES CXX)

# Set C++20 for all targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Uncomment the following line to display the exact compiler commands
# set(CMAKE_VERBOSE_MAKEFILE ON)

# Define include directories
set(INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/algorithm
        ${CMAKE_SOURCE_DIR}/simulator
        )

# Add your algorithm libraries
add_library(Algorithm_212346076_207177197_B MODULE
        algorithm/Algorithm_212346076_207177197_B.cpp)
target_include_directories(Algorithm_212346076_207177197_B PUBLIC ${INCLUDE_DIRS})

# Add your second algorithm library here
# add_library(Algorithm_212346076_207177197_A MODULE
#             algorithm/Algorithm_212346076_207177197_A.cpp)
# target_include_directories(Algorithm_212346076_207177197_A PUBLIC ${INCLUDE_DIRS})

# Add the main executable
add_executable(main
        simulator/main.cpp
        simulator/AlgorithmRegistrar.cpp
        simulator/Simulation.cpp
        simulator/House.cpp
        simulator/Vacuum.cpp
        common/PositionUtils.cpp)

# Set include directories for the main executable
target_include_directories(main PUBLIC ${INCLUDE_DIRS})

# Link against dl library for dynamic loading
target_link_libraries(main dl)

# Add the -rdynamic flag to the linker options
target_link_options(main PUBLIC "-rdynamic")

# If you have any other shared libraries that the main executable needs to link against, add them here
# target_link_libraries(main other_library)