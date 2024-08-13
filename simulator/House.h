#ifndef HOUSE_H
#define HOUSE_H

#include "../common/states.h"
#include <vector>
#include <string>

class House {
public:
    House(const std::vector<std::string>& layout_v);

    // Getters
    int getRows() const;
    int getCols() const;
    Position getDockingStation() const;
    int getCell(const Position& pos) const;

    // Methods for assignment 3
    bool isWall(const Position& pos) const;
    int getDirtLevel(const Position& pos) const;
    void cleanCell(const Position& pos);
    bool isValidPosition(const Position& pos) const;
    bool isInDock(const Position& pos) const;

    // Utility methods
    int getTotalDirt() const;
    bool isHouseClean() const;
    void printMatrix() const;
    void printInfo() const;
    void printLayout() const;

private:
    std::vector<std::vector<int>> house_matrix;
    int rows;
    int cols;
    Position dockingStation;
    int total_dirt;

    void addWallsPadding(std::vector<std::string>& layout_v);
    void initializeMatrix(const std::vector<std::string>& layout_v);
    void findDockingStation();
    void updateDirtCount();
};

#endif // HOUSE_H