#include "House.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

House::House(const std::vector<std::string>& layout_v)
        : dockingStationRow(-1), dockingStationCol(-1), total_dirt(0) {
    std::vector<std::string> padded_layout = layout_v;
    addWallsPadding(padded_layout);
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
                house_matrix[i][j] = -1;
            } else if (cell >= '1' && cell <= '9') {
                house_matrix[i][j] = cell - '0';
                total_dirt += house_matrix[i][j];
            } else if (cell == 'D') {
                dockingStationRow = i;
                dockingStationCol = j;
                house_matrix[i][j] = -20;
            } else {
                house_matrix[i][j] = 0;
            }
        }
    }
}

void House::findDockingStation() {
    if (dockingStationRow == -1 || dockingStationCol == -1) {
        throw std::runtime_error("Docking station 'D' not found in layout");
    }
}

void House::updateDirtCount() {
    total_dirt = 0;
    for (const auto& row : house_matrix) {
        for (int cell : row) {
            if (cell > 0 && cell < 20) {
                total_dirt += cell;
            }
        }
    }
}

int House::getRows() const { return rows; }
int House::getCols() const { return cols; }
int House::getDockingStationRow() const { return dockingStationRow; }
int House::getDockingStationCol() const { return dockingStationCol; }

int House::getCell(int row, int col) const {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return -1; // Boundary walls represented by -1
    }
    return house_matrix[row][col];
}

bool House::isWall(int row, int col) const {
    return getCell(row, col) == -1;
}

int House::getDirtLevel(int row, int col) const {
    int cell = getCell(row, col);
    return (cell > 0 && cell < 20) ? cell : 0;
}

void House::cleanCell(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        if (house_matrix[row][col] > 0 && house_matrix[row][col] < 20) {
            house_matrix[row][col]--;
            total_dirt--;
            std::cout << "Cleaned cell at (" << row << ", " << col
                      << "). New dirt level: " << house_matrix[row][col] << std::endl;
        }
    }
}

bool House::isValidPosition(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols && !isWall(row, col);
}

bool House::isInDock(int row, int col) const {
    return row == dockingStationRow && col == dockingStationCol;
}

int House::getTotalDirt() const {
    return total_dirt;
}

bool House::isHouseClean() const {
    return total_dirt == 0;
}

void House::printMatrix() const {
    std::cout << "House matrix:\n";
    std::cout << "Docking station: (" << dockingStationRow << ", " << dockingStationCol << ")\n";

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