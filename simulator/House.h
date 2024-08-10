#ifndef HOUSE_H
#define HOUSE_H

#include <vector>
#include <string>

class House {
public:
    House(const std::vector<std::string>& layout_v);

    // Getters
    int getRows() const;
    int getCols() const;
    int getDockingStationRow() const;
    int getDockingStationCol() const;
    int getCell(int row, int col) const;

    // Methods for assignment 3
    bool isWall(int row, int col) const;
    int getDirtLevel(int row, int col) const;
    void cleanCell(int row, int col);
    bool isValidPosition(int row, int col) const;
    bool isInDock(int row, int col) const;

    // Utility methods
    int getTotalDirt() const;
    bool isHouseClean() const;
    void printMatrix() const;

private:
    std::vector<std::vector<int>> house_matrix;
    int rows;
    int cols;
    int dockingStationRow;
    int dockingStationCol;
    int total_dirt;

    void addWallsPadding(std::vector<std::string>& layout_v);
    void initializeMatrix(const std::vector<std::string>& layout_v);
    void findDockingStation();
    void updateDirtCount();
};

#endif // HOUSE_H