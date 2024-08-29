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
    bool isAreaUnexplored(const Position pos);
    void removeFromUnexplored(const Position pos);
    void updateAdjacentArea(Direction dir, Position position, bool isWall);
    Position getClosestUnexploredArea(Position position);
    bool hasMoreDirtyAreas() const;

    std::stack<Direction> getShortestPath(std::pair<int, int> src,
                                          std::pair<int, int> dst,
                                          bool search);

    std::stack<Direction> getShortestPath_A(std::pair<int, int> src,
                                            std::pair<int, int> dst,
                                            bool search);

    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> point);

private:
    std::map<Position, std::pair<int,int>> mapped_areas_; // first: dirt level, second: curr distance from docking station
    std::map<Position, bool> unexplored_areas_;
    int total_dirt_;
};

#endif //VACUUM_FINAL_EXPLORER_H
