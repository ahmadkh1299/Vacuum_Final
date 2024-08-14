//
// Created by Mariam on 8/12/2024.
//

#include "AlgorithmAstar212346076A.h"
#include "AlgorithmAstar212346076A.h"
#include "../algorithm/AlgorithmRegistration.h"
#include <cmath>
#include <algorithm>
#include <limits>



AlgorithmAstar212346076A::AlgorithmAstar212346076A()
        : maxSteps(0), sensor(nullptr), currentRow(0), currentCol(0),
          dockRow(0), dockCol(0), currentBattery(0), maxBattery(0), returningToDock(false) {}

void AlgorithmAstar212346076A::setMaxSteps(std::size_t maxSteps) {
    this->maxSteps = maxSteps;
    dirtMap.resize(maxSteps, std::vector<int>(maxSteps, -1));
    visitedMap.resize(maxSteps, std::vector<bool>(maxSteps, false));
}

void AlgorithmAstar212346076A::setSensor(const SensorInterface& sensor) {
    this->sensor = &sensor;
    currentBattery = sensor.getBatteryState();
    maxBattery = currentBattery;
}

Step AlgorithmAstar212346076A::nextStep() {
    if (pathSteps.empty()) {
        updateMaps();

        if (returningToDock) {
            if (currentRow == dockRow && currentCol == dockCol) {
                returningToDock = false;
                return Step::Stay; // Stay at dock to charge
            }
            planReturnToDock();
        } else if (needsRecharge()) {
            returningToDock = true;
            planReturnToDock();
        } else {
            auto [dirtRow, dirtCol] = findNearestDirt();
            if (dirtRow == -1 || dirtCol == -1) {
                if (isFullyExplored()) {
                    returningToDock = true;
                    planReturnToDock();
                } else {
                    exploreUnknownAreas();
                }
            } else if (canSafelyReachTarget(dirtRow, dirtCol)) {
                pathSteps = std::queue<Step>(findPath(currentRow, currentCol, dirtRow, dirtCol));
            } else {
                returningToDock = true;
                planReturnToDock();
            }
        }
    }

    if (!pathSteps.empty()) {
        Step nextStep = pathSteps.front();
        pathSteps.pop();
        return executeStep(nextStep);
    }

    return Step::Stay;
}

std::vector<Step> AlgorithmAstar212346076A::findPath(int startRow, int startCol, int goalRow, int goalCol) {
    std::vector<std::vector<std::pair<int, int>>> came_from(maxSteps, std::vector<std::pair<int, int>>(maxSteps, {-1, -1}));
    std::vector<std::vector<int>> g_score(maxSteps, std::vector<int>(maxSteps, std::numeric_limits<int>::max()));

    auto compare = [&](std::pair<int, int> a, std::pair<int, int> b) {
        int f_a = g_score[a.first][a.second] + manhattanDistance(a.first, a.second, goalRow, goalCol);
        int f_b = g_score[b.first][b.second] + manhattanDistance(b.first, b.second, goalRow, goalCol);
        return f_a > f_b;
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(compare)> open_set(compare);

    g_score[startRow][startCol] = 0;
    open_set.push({startRow, startCol});

    while (!open_set.empty()) {
        auto current = open_set.top();
        open_set.pop();

        if (current.first == goalRow && current.second == goalCol) {
            // Reconstruct path
            std::vector<Step> path;
            while (current.first != startRow || current.second != startCol) {
                auto prev = came_from[current.first][current.second];
                if (prev.first < current.first) path.push_back(Step::South);
                else if (prev.first > current.first) path.push_back(Step::North);
                else if (prev.second < current.second) path.push_back(Step::East);
                else path.push_back(Step::West);
                current = prev;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto neighbor : getNeighbors(current.first, current.second)) {
            int tentative_g_score = g_score[current.first][current.second] + 1;
            if (tentative_g_score < g_score[neighbor.first][neighbor.second]) {
                came_from[neighbor.first][neighbor.second] = current;
                g_score[neighbor.first][neighbor.second] = tentative_g_score;
                open_set.push(neighbor);
            }
        }
    }

    return {}; // No path found
}

int AlgorithmAstar212346076A::manhattanDistance(int row1, int col1, int row2, int col2) const {
    return std::abs(row1 - row2) + std::abs(col1 - col2);
}

std::vector<std::pair<int, int>> AlgorithmAstar212346076A::getNeighbors(int row, int col) const {
    std::vector<std::pair<int, int>> neighbors;
    const int dr[] = {-1, 0, 1, 0};
    const int dc[] = {0, 1, 0, -1};
    for (int i = 0; i < 4; ++i) {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        if (newRow >= 0 && newRow < maxSteps && newCol >= 0 && newCol < maxSteps && !sensor->isWall(static_cast<Direction>(i))) {
            neighbors.push_back({newRow, newCol});
        }
    }
    return neighbors;
}

std::pair<int, int> AlgorithmAstar212346076A::findNearestDirt() const {
    int nearestRow = -1, nearestCol = -1;
    int minDistance = std::numeric_limits<int>::max();

    for (int i = 0; i < maxSteps; ++i) {
        for (int j = 0; j < maxSteps; ++j) {
            if (dirtMap[i][j] > 0) {
                int distance = manhattanDistance(currentRow, currentCol, i, j);
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestRow = i;
                    nearestCol = j;
                }
            }
        }
    }

    return {nearestRow, nearestCol};
}

void AlgorithmAstar212346076A::updateMaps() {
    dirtMap[currentRow][currentCol] = sensor->dirtLevel();
    visitedMap[currentRow][currentCol] = true;
}

bool AlgorithmAstar212346076A::canSafelyReachTarget(int targetRow, int targetCol) const {
    int estimatedCost = estimateRoundTripCost(targetRow, targetCol);
    return currentBattery > estimatedCost + maxBattery / 10; // 10% battery safety margin
}

int AlgorithmAstar212346076A::estimateRoundTripCost(int targetRow, int targetCol) const {
    return manhattanDistance(currentRow, currentCol, targetRow, targetCol) +
           manhattanDistance(targetRow, targetCol, dockRow, dockCol);
}

void AlgorithmAstar212346076A::planReturnToDock() {
    pathSteps = std::queue<Step>(findPath(currentRow, currentCol, dockRow, dockCol));
}

void AlgorithmAstar212346076A::updateBattery() {
    currentBattery--;
    if (currentRow == dockRow && currentCol == dockCol) {
        currentBattery = std::min(currentBattery + maxBattery / 20, maxBattery);
    }
}

bool AlgorithmAstar212346076A::needsRecharge() const {
    return currentBattery < maxBattery / 5; // Return to dock at 20% battery
}

Step AlgorithmAstar212346076A::executeStep(Step step) {
    switch (step) {
        case Step::North: currentRow--; break;
        case Step::South: currentRow++; break;
        case Step::East: currentCol++; break;
        case Step::West: currentCol--; break;
        default: break;
    }
    updateBattery();
    clean();
    return step;
}

void AlgorithmAstar212346076A::clean() {
    if (dirtMap[currentRow][currentCol] > 0) {
        dirtMap[currentRow][currentCol]--;
    }
}

void AlgorithmAstar212346076A::exploreUnknownAreas() {
    for (int i = 0; i < maxSteps; ++i) {
        for (int j = 0; j < maxSteps; ++j) {
            if (!visitedMap[i][j] && canSafelyReachTarget(i, j)) {
                pathSteps = std::queue<Step>(findPath(currentRow, currentCol, i, j));
                return;
            }
        }
    }
    // If no unknown areas can be safely reached, return to dock
    returningToDock = true;
    planReturnToDock();
}

bool AlgorithmAstar212346076A::isFullyExplored() const {
    for (const auto& row : visitedMap) {
        for (bool visited : row) {
            if (!visited) return false;
        }
    }
    return true;
}
REGISTER_ALGORITHM(AlgorithmAstar212346076A)