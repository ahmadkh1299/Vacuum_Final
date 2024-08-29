#include "House.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

House::House(const std::vector<std::string>& layout_v, const std::string& name)
        : dockingStation({-1, -1}), total_dirt(0), house_name(name),dirt_count(0) {  // Save the house name
    std::vector<std::string> padded_layout = layout_v;
    //addWallsPadding(padded_layout);
    initializeMatrix(padded_layout);
    findDockingStation();
    updateDirtCount();
}

void House::addWallsPadding(std::vector<std::string>& layout_v) {
    if (layout_v.empty()) return;

    // Find the maximum row length
    size_t max_length = 0;
    for (const auto& row : layout_v) {
        max_length = std::max(max_length, row.length());
    }

    // Pad each row to the maximum length and add side walls
    for (auto& row : layout_v) {
        row.resize(max_length, ' ');
        row.insert(row.begin(), 'W');
        row.push_back('W');
    }

    // Add top and bottom walls
    layout_v.insert(layout_v.begin(), std::string(max_length + 2, 'W'));
    layout_v.push_back(std::string(max_length + 2, 'W'));
}

void House::initializeMatrix(const std::vector<std::string>& layout_v) {
    rows = static_cast<int>(layout_v.size());
    cols = static_cast<int>(layout_v[0].size());
    house_matrix.resize(rows, std::vector<int>(cols, 0));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            char cell = layout_v[i][j];
            if (cell == 'W') {
                house_matrix[i][j] = -1; // Wall
            } else if (cell >= '1' && cell <= '9') {
                house_matrix[i][j] = cell - '0'; // Dirt level

                // Check if the cell is not surrounded by walls
                bool surrounded_by_walls = true;
                std::vector<Position> neighbors = {
                        {i-1, j}, {i+1, j}, {i, j-1}, {i, j+1}
                };

                for (const auto& neighbor : neighbors) {
                    if (neighbor.r >= 0 && neighbor.r < rows &&
                        neighbor.c >= 0 && neighbor.c < cols &&
                        layout_v[neighbor.r][neighbor.c] != 'W') {
                        surrounded_by_walls = false;
                        break;
                    }
                }

                // Only add the dirt to total_dirt if not surrounded by walls
                if (!surrounded_by_walls) {
                    total_dirt += house_matrix[i][j];
                }
            } else if (cell == 'D') {
                dockingStation = {i, j};
                std::cout << "Docking station found at (" << i << ", " << j << ")" << std::endl;
                house_matrix[i][j] = -20; // Docking station
            } else {
                house_matrix[i][j] = 0; // Empty space
            }
        }
    }
}


void House::findDockingStation() {
    if (dockingStation.r == -1 || dockingStation.c == -1) {
        throw std::runtime_error("Docking station 'D' not found in layout");
    }
}

void House::updateDirtCount() {
    dirt_count = 0;
    for (const auto& row : house_matrix) {
        for (int cell : row) {
            if (cell > 0 && cell < 20) {
                dirt_count += cell;
            }
        }
    }
}

int House::getRows() const {
    return rows;
}

int House::getCols() const {
    return cols;
}

Position House::getDockingStation() const {
    return dockingStation;
}

int House::getCell(const Position& pos) const {
    if (pos.r < 0 || pos.r >= rows || pos.c < 0 || pos.c >= cols) {
        return -1; // Boundary walls represented by -1
    }
    return house_matrix[pos.r][pos.c];
}

void House::printHouseMatrix() const {
    std::cout << "House Matrix:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            char displayChar;
            switch(house_matrix[i][j]) {
                case -1:
                    displayChar = 'W';  // Wall
                    break;
                case -20:
                    displayChar = 'D';  // Docking station
                    break;
                case 0:
                    displayChar = '0';  // Empty space
                    break;
                default:
                    displayChar = '0' + house_matrix[i][j];  // Dirt level (1-9)
                    break;
            }
            std::cout << displayChar << ' ';
        }
        std::cout << std::endl;
    }
}

std::string House::getName() const {
    return house_name;
}

bool House::isWall(const Position& pos) const {
    return getCell(pos) == -1;
}

int House::getDirtLevel(const Position& pos) const {
    int cell = getCell(pos);
    if (pos == dockingStation) {
        return -20;
    }
    return (cell > 0 && cell < 20) ? cell : 0;
}

void House::cleanCell(const Position& pos) {
    if (pos.r >= 0 && pos.r < rows && pos.c >= 0 && pos.c < cols) {
        if (house_matrix[pos.r][pos.c] > 0 && house_matrix[pos.r][pos.c] < 10) {
            std::cout << "Cleaned cell at (" << pos.r << ", " << pos.c
                      << "). dirt level: " << house_matrix[pos.r][pos.c] << std::endl;
            house_matrix[pos.r][pos.c]--;
            total_dirt--;
            std::cout << "Cleaned cell at (" << pos.r << ", " << pos.c
                      << "). New dirt level: " << house_matrix[pos.r][pos.c] << std::endl;
        }
    }
}

bool House::isValidPosition(const Position& pos) const {
    return pos.r >= 0 && pos.r < rows && pos.c >= 0 && pos.c < cols && !isWall(pos);
}

bool House::isInDock(const Position& pos) const {
    return pos.r == dockingStation.r && pos.c == dockingStation.c;
}

int House::getTotalDirt() const {
    return total_dirt;
}

bool House::isHouseClean() const {
    return total_dirt == 0;
}

void House::printMatrix() const {
    std::cout << "House matrix:\n";
    std::cout << "Docking station: (" << dockingStation.r << ", " << dockingStation.c << ")\n";

    for (const auto& row : house_matrix) {
        for (int cell : row) {
            if (cell == -1) {
                std::cout << "W ";
            } else if (cell == -20) {
                std::cout << "D ";
            } else {
                std::cout << cell << ' ';
            }
        }
        std::cout << '\n';
    }
}

void House::printLayout() const {
    std::cout << "House Layout:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            switch(house_matrix[i][j]) {
                case 0: std::cout << ' '; break; // Empty
                case -1: std::cout << 'W'; break; // Wall
                case -20: std::cout << 'D'; break; // Docking station
                default: std::cout << house_matrix[i][j]; // Dirt level
            }
        }
        std::cout << std::endl;
    }
}

void House::printInfo() const {
    std::cout << "House Info:" << std::endl;
    std::cout << "Rows: " << rows << std::endl;
    std::cout << "Columns: " << cols << std::endl;
    std::cout << "Docking Station: (" << dockingStation.r << ", " << dockingStation.c << ")" << std::endl;
    std::cout << "Total Dirt: " << total_dirt << std::endl;
}