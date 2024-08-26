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

# Function to add an algorithm library
function(add_algorithm_library NAME)
    add_library(${NAME} SHARED
        algorithm/${NAME}.cpp
        simulator/Explorer.cpp
        simulator/House.cpp
        simulator/AlgorithmRegistrar.cpp
        common/PositionUtils.cpp
        common/SensorImpl.cpp
        simulator/Vacuum.cpp


    )
    target_include_directories(${NAME} PUBLIC ${INCLUDE_DIRS})
    set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction()

# Add your algorithm libraries
add_algorithm_library(Algorithm_212346076_207177197_B)
add_algorithm_library(AlgorithmDFS)

# Add the main executable
add_executable(main
    simulator/main.cpp
    simulator/AlgorithmRegistrar.cpp
    simulator/Simulation.cpp
    simulator/House.cpp
    simulator/Vacuum.cpp
    simulator/Explorer.cpp
    common/PositionUtils.cpp
    common/SensorImpl.cpp
)

# Find and link pthread
find_package(Threads REQUIRED)
target_link_libraries(main PRIVATE Threads::Threads)

# Set include directories for the main executable
target_include_directories(main PUBLIC ${INCLUDE_DIRS})

# Link against the necessary libraries
if (UNIX AND NOT APPLE)
    target_link_libraries(main PRIVATE dl)
    target_link_options(main PRIVATE "-rdynamic")
endif()

# Installation rules
install(TARGETS main
    RUNTIME DESTINATION bin
)

install(TARGETS Algorithm_212346076_207177197_B AlgorithmDFS
    LIBRARY DESTINATION lib
)