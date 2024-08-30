//
// Created by 97250 on 8/13/2024.
//

#include <queue>
#include "Explorer.h"
#include "../common/PositionUtils.h"

Explorer::Explorer(){
    
}

bool Explorer::explored(const Position pos) const {
    return mapped_areas_.count(pos) != 0;
}
int Explorer::getDirtLevel(const Position pos) {
    if (explored(pos)) {
        return mapped_areas_.at(pos).first;
    }
    std::cout << "ERROR!! " << __FUNCTION__ << " position does not exist." << std::endl;
    return -2; // Consider defining error codes in a separate header
}

// Get the distance from the docking station to a specific position
int Explorer::getDistance(const Position pos) {
    if (mapped_areas_.count(pos) != 0) {
        return mapped_areas_[pos].second;
    }
    return -1;
}

// Set the distance from the docking station to a specific position
void Explorer::setDistance(const Position pos, int distance) {
        mapped_areas_[pos].second = distance;
}


// Set the dirt level for a specific position
void Explorer::setDirtLevel(const Position pos, int dirtLevel) {
    if (mapped_areas_.count(pos) != 0 && mapped_areas_[pos].first > 0 && mapped_areas_[pos].first <= MAXIMUM_DIRT) {
        total_dirt_ -= mapped_areas_[pos].first;
    }

    mapped_areas_[pos].first = dirtLevel;
    total_dirt_ += (dirtLevel >= 0 && dirtLevel <= MAXIMUM_DIRT) ? dirtLevel : 0;
}

// Check if a position is a wall
bool Explorer::isWall(const Position pos) {
    return mapped_areas_[pos].first == static_cast<int>(LocType::Wall);
}

// Check if a position is the docking station
bool Explorer::isDockingStation(const Position pos) {
    return mapped_areas_[pos].first == static_cast<int>(LocType::Dock);
}

// Perform cleaning at a specific position
void Explorer::performCleaning(const Position pos) {
    if (mapped_areas_[pos].first > 0 && mapped_areas_[pos].first <= MAXIMUM_DIRT) {
        mapped_areas_[pos].first--;
        total_dirt_--;
    }
}

// Update the dirt level at a position and then clean it
void Explorer::updateDirtAndClean(const Position pos, int dirtLevel) {
    if (mapped_areas_[pos].first > 0 && mapped_areas_[pos].first <= MAXIMUM_DIRT) {
        total_dirt_ -= mapped_areas_[pos].first;
    }

    mapped_areas_[pos].first = dirtLevel;
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
        mapped_areas_[adjacentPosition].first = static_cast<int>(LocType::Wall);
    } else {
        if (mapped_areas_.count(adjacentPosition) == 0) {
            unexplored_areas_[adjacentPosition];
        }
    }
}

//function to find the path from src to dst using A* algorithm, or to the closest dirt/unexplored area if search is true
std::stack<Direction> Explorer::getShortestPath_A(std::pair<int, int> src, std::pair<int, int> dst, bool search) {
    std::stack<Direction> path;
    std::priority_queue<Position, std::vector<Position>, std::greater<Position>> pq;
    std::map<Position, bool> visited;
    std::map<Position, Position> parent;
    std::map<Position, int> g;
    std::map<Position, int> h;
    std::map<Position, int> f;
    pq.push(Position{src.first, src.second});
    g[Position{src.first, src.second}] = 0;
    h[Position{src.first, src.second}] = abs(src.first - dst.first) + abs(src.second - dst.second);
    f[Position{src.first, src.second}] = g[Position{src.first, src.second}] + h[Position{src.first, src.second}];
    visited[Position{src.first, src.second}] = true;
    bool found = false;
    while (!pq.empty()) {
        Position t = pq.top();
        pq.pop();
        if (!search && t.r == dst.first && t.c == dst.second) {
            found = true;
            break;
        }

        for (const auto& v : getNeighbors({t.r, t.c})) {
            Position neighbor = {v.first, v.second};
            if (visited.count(neighbor) == 0) {
                g[neighbor] = g[t] + 1;
                h[neighbor] = abs(neighbor.r - dst.first) + abs(neighbor.c - dst.second);
                f[neighbor] = g[neighbor] + h[neighbor];
                pq.push(neighbor);
                visited[neighbor] = true;
                parent[neighbor] = t;
            }
        }
        // Check for search mode conditions
        if (search && ((mapped_areas_.count(t) != 0 && mapped_areas_[t].first > 0) || unexplored_areas_.count(t) != 0)) {
            found = true;
            break;
        }

    }
    if (found) {
        Position v = search ? pq.top() : Position{dst.first, dst.second};
        while (v != Position{src.first, src.second}) {
            path.push(PositionUtils::findDirection(parent[v], v));
            v = parent[v];
        }
    } else {
        std::queue<Position> q;
        std::map<Position, bool> visited;
        std::map<Position, Position> parent;
        q.push({src.first, src.second});
        visited[{src.first, src.second}] = true;
        while (!q.empty()) {
            Position t = q.front();
            q.pop();
            for (const auto& v : getNeighbors({t.r, t.c})) {
                Position neighbor = {v.first, v.second};
                if (visited.count(neighbor) == 0) {
                    q.push(neighbor);
                    visited[neighbor] = true;
                    parent[neighbor] = t;
                }
            }
            if (search) {
                if (!((mapped_areas_.count(t) != 0 && mapped_areas_[t].first > 0) || unexplored_areas_.count(t) != 0)) {
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
    }
    if (path.empty()) {
        // Log this event or handle it appropriately
        std::cerr << "Warning: Empty path returned for src: (" << src.first << "," << src.second 
                  << ") to dst: (" << dst.first << "," << dst.second << ")" << std::endl;
    }
    return path;
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
            if (!((mapped_areas_.count(t) != 0 && mapped_areas_[t].first > 0) || unexplored_areas_.count(t) != 0)) {
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
    if (path.empty()) {
        // Log this event or handle it appropriately
        std::cerr << "Warning: Empty path returned for src: (" << src.first << "," << src.second 
                  << ") to dst: (" << dst.first << "," << dst.second << ")" << std::endl;
    }
    return path;}

bool Explorer::hasMoreDirtyAreas() const {
    for (const auto& area : mapped_areas_) {
        if (area.second.first > 0) {  // Check if there's any dirt left in any mapped area
            return true;
        }
    }
    return false;
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
/*std::stack<Direction> Explorer::findPathToDock(const Position& start, const Position& dock) {
    std::cout << "Finding path to dock from (" << start.r << "," << start.c 
              << ") to (" << dock.r << "," << dock.c << ")" << std::endl;

    std::queue<Position> q;
    std::map<Position, Position> parent;
    std::map<Position, bool> visited;

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        Position current = q.front();
        q.pop();

        if (current == dock) {
            return reconstructPath(parent, current, start);
        }

        std::vector<std::pair<int, int>> neighbors = {
            {current.r-1, current.c}, {current.r+1, current.c},
            {current.r, current.c-1}, {current.r, current.c+1}
        };

        for (const auto& [r, c] : neighbors) {
            Position next{r, c};
            if (visited[next]) continue;

            // Check if the position is within the known map or unexplored
            if (mapped_areas_.count(next) || unexplored_areas_.count(next)) {
                // If it's mapped, make sure it's not a wall
                if (mapped_areas_.count(next) == 0 || !isWall(next)) {
                    q.push(next);
                    visited[next] = true;
                    parent[next] = current;
                }
            }
        }
    }

    std::cout << "No path found to dock" << std::endl;
    return std::stack<Direction>();
}

std::stack<Direction> Explorer::findPathToDock(const Position& start, const Position& dock) {
    std::cout << "Finding path to dock from (" << start.r << "," << start.c 
              << ") to (" << dock.r << "," << dock.c << ")" << std::endl;

    std::queue<Position> q;
    std::map<Position, Position> parent;
    std::set<Position> visited;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        Position current = q.front();
        q.pop();

        if (current == dock) {
            return reconstructPath(parent, current, start);
        }

        std::vector<std::pair<int, int>> neighbors = {
            {current.r-1, current.c}, {current.r+1, current.c},
            {current.r, current.c-1}, {current.r, current.c+1}
        };

        for (const auto& [r, c] : neighbors) {
            Position next{r, c};
            if (visited.count(next)) continue;

            // Check if the position is within the known map or unexplored
            if (mapped_areas_.count(next) || unexplored_areas_.count(next)) {
                // If it's mapped, make sure it's not a wall
                if (mapped_areas_.count(next) == 0 || !isWall(next)) {
                    q.push(next);
                    visited.insert(next);
                    parent[next] = current;
                }
            }
        }
    }

    std::cout << "No path found to dock" << std::endl;
    return std::stack<Direction>();
}
std::stack<Direction> Explorer::reconstructPath(const std::map<Position, Position>& parent, Position current, Position start) {
    std::stack<Direction> path;
    while (current != start) {
        Position prev = parent.at(current);
        path.push(PositionUtils::findDirection(prev, current));
        current = prev;
    }
    return path;
}*/
Position Explorer::getClosestUnexploredArea(Position position) {
    int Dist = 0;
    int min = INT_MAX;
    Position pos = {-20, -20};
    for (const auto& area : unexplored_areas_) {
        Dist = getShortestPath_A({position.r, position.c}, {area.first.r, area.first.c}, false).size();
        if (Dist < min) {
            min = Dist;
            pos = area.first;
        }
    }
    return pos;
}