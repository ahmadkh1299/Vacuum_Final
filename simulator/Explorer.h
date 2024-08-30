//
// Created by 97250 on 8/13/2024.
//

#ifndef VACUUM_FINAL_EXPLORER_H
#define VACUUM_FINAL_EXPLORER_H

#include <stack>
#include <map>
#include "../common/states.h"
#include "../common/enums.h"
#include "../common/PositionUtils.h"
#include <climits>



#include <set>

class Explorer {
public:
    Explorer();
    ~Explorer() = default;
    bool explored(const Position pos) const;
    int getDirtLevel(const Position pos);
    void setDirtLevel(const Position pos, int dirtLevel);
    int getDistance(const Position pos);
    void setDistance(const Position pos, int distance);
    bool isWall(const Position pos);
    bool isDockingStation(const Position pos);
    void performCleaning(const Position pos);
    void updateDirtAndClean(const Position pos, int dirtLevel);
    bool areAllAreasExplored();
     std::vector<Position> getUnexploredAreas() const {
        std::vector<Position> unexplored;
        for (const auto& [pos, _] : unexplored_areas_) {
            unexplored.push_back(pos);
        }
        return unexplored;
    }
    bool isAreaUnexplored(const Position pos);
    void removeFromUnexplored(const Position pos);
    void updateAdjacentArea(Direction dir, Position position, bool isWall);

    bool hasMoreDirtyAreas() const;

    std::stack<Direction> getShortestPath(std::pair<int, int> src,
                                          std::pair<int, int> dst,
                                          bool search);

    std::stack<Direction> getShortestPath_A(std::pair<int, int> src,
                                          std::pair<int, int> dst,
                                          bool search);

    int manhattanDistance(const Position &a, const Position &b);

    std::stack<Direction> reconstructPath(const std::map<Position, Position> &parent, Position current, Position start);

    //int heuristic(const Position &a, const Position &b) const;

    //bool isValidPosition(const Position &pos) const;
    //std::stack<Direction> reconstructPath(const std::map<Position, Position> &cameFrom, const Position &current, const Position &start) const;

    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> point);
    Position getClosestUnexploredArea(Position position);
    std::stack<Direction> findPathToDock(const Position &start, const Position &dock);
    std::map<Position, std::pair<int, int>> mapped_areas_; // first: dirt level, second: curr distance from docking station
    std::map<Position, bool> unexplored_areas_;

private:
    
    int total_dirt_;
};

#endif //VACUUM_FINAL_EXPLORER_H
