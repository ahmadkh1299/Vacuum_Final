//
// Created by 97250 on 8/13/2024.
//

#include <queue>
#include "Explorer.h"
#include "../common/PositionUtils.h"


bool Explorer::explored(const Position pos) const {
    return mapped_areas_.count(pos) != 0;
}
int Explorer::getDirtLevel(const Position pos) {
    if (explored(pos)) {
        return mapped_areas_.at(pos);
    }
    std::cout << "ERROR!! " << __FUNCTION__ << " position does not exist." << std::endl;
    return -2; // Consider defining error codes in a separate header
}

// Set the dirt level for a specific position
void Explorer::setDirtLevel(const Position pos, int dirtLevel) {
    if (mapped_areas_.count(pos) != 0 && mapped_areas_[pos] > 0 && mapped_areas_[pos] <= MAXIMUM_DIRT) {
        total_dirt_ -= mapped_areas_[pos];
    }

    mapped_areas_[pos] = dirtLevel;
    total_dirt_ += (dirtLevel >= 0 && dirtLevel <= MAXIMUM_DIRT) ? dirtLevel : 0;
}

// Check if a position is a wall
bool Explorer::isWall(const Position pos) {
    return mapped_areas_[pos] == static_cast<int>(LocType::Wall);
}

// Check if a position is the docking station
bool Explorer::isDockingStation(const Position pos) {
    return mapped_areas_[pos] == static_cast<int>(LocType::Dock);
}

// Perform cleaning at a specific position
void Explorer::performCleaning(const Position pos) {
    if (mapped_areas_[pos] > 0 && mapped_areas_[pos] <= MAXIMUM_DIRT) {
        mapped_areas_[pos]--;
        total_dirt_--;
    }
}

// Update the dirt level at a position and then clean it
void Explorer::updateDirtAndClean(const Position pos, int dirtLevel) {
    if (mapped_areas_[pos] > 0 && mapped_areas_[pos] <= MAXIMUM_DIRT) {
        total_dirt_ -= mapped_areas_[pos];
    }

    mapped_areas_[pos] = dirtLevel;
    total_dirt_ += (dirtLevel >= 0 && dirtLevel <= MAXIMUM_DIRT) ? dirtLevel : 0;

    performCleaning(pos);
}

// Check if there are any unexplored areas left
bool Explorer::areAllAreasExplored() {
    return unexplored_areas_.empty();
}

// Check if a specific position is unexplored
bool Explorer::isAreaUnexplored(const Position pos) {
    return unexplored_areas_.count(pos) != 0;
}

// Remove a position from the unexplored areas
void Explorer::removeFromUnexplored(const Position pos) {
    if (isAreaUnexplored(pos)) {
        unexplored_areas_.erase(pos);
    }
}

// Update the information about an adjacent area
void Explorer::updateAdjacentArea(Direction dir, Position position, bool isWall) {
    Position adjacentPosition = PositionUtils::movePosition(position, dir);
    if (isWall) {
        mapped_areas_[adjacentPosition] = static_cast<int>(LocType::Wall);
    } else {
        if (mapped_areas_.count(adjacentPosition) == 0) {
            unexplored_areas_[adjacentPosition];
        }
    }
}

// Get the shortest path from source to destination, or to the closest dirt/unexplored area if search is true
std::stack<Direction> Explorer::getShortestPath(std::pair<int, int> src, std::pair<int, int> dst, bool search) {
    std::stack<Direction> path;

    std::queue<Position> q;
    std::map<Position, bool> visited;
    std::map<Position, Position> parent;

    q.push({src.first, src.second});
    visited[{src.first, src.second}] = true;

    bool found = false;

    while (!q.empty()) {
        Position t = q.front();
        q.pop();

        for (const auto& v : getNeighbors({t.r, t.c})) {
            Position neighbor = {v.first, v.second};
            if (visited.count(neighbor) == 0) { // If not visited
                q.push(neighbor);
                visited[neighbor] = true;
                parent[neighbor] = t;
            }
        }

        if (search) {
            if (!((mapped_areas_.count(t) != 0 && mapped_areas_[t] > 0) || unexplored_areas_.count(t) != 0)) {
                continue;
            }
        }

        if (!search) {
            if ((!path.empty() || (t.r != dst.first || t.c != dst.second))) {
                continue;
            }
        }

        Position v = t;
        while (v != Position{src.first, src.second}) {
            path.push(PositionUtils::findDirection(parent[v], v));
            v = parent[v];
        }
        break;
    }
    return path;
}

// Get the neighboring positions for a given point
std::vector<std::pair<int, int>> Explorer::getNeighbors(std::pair<int, int> point) {
    static std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, 1}, {0, -1}
    };

    std::vector<std::pair<int, int>> neighbors;

    for (const auto& dir : directions) {
        std::pair<int, int> temp = {point.first + dir.first, point.second + dir.second};

        if ((mapped_areas_.count({temp.first, temp.second}) != 0 && !isWall({temp.first, temp.second})) ||
            unexplored_areas_.count({temp.first, temp.second}) != 0) {
            neighbors.push_back(temp);
        }
    }
    return neighbors;
}
