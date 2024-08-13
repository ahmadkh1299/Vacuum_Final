//
// Created by Mariam on 8/12/2024.
//

#ifndef VACUUM_FINAL_ALGORITHMASTAR212346076A_H
#define VACUUM_FINAL_ALGORITHMASTAR212346076A_H


#include "../common/AbstractAlgorithm.h"
#include "../common/SensorInterface.h"
#include "../common/enums.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <memory>

class AlgorithmAstar212346076A : public AbstractAlgorithm {
public:
    AlgorithmAstar212346076A();
    ~AlgorithmAstar212346076A() override = default;

    // Required overrides from AbstractAlgorithm
    void setMaxSteps(std::size_t maxSteps) override;
    void setSensor(const SensorInterface& sensor) override;
    Step nextStep() override;

private:
    std::size_t maxSteps;
    const SensorInterface* sensor;
    std::vector<std::vector<int>> dirtMap;
    std::vector<std::vector<bool>> visitedMap;
    int currentRow, currentCol;
    int dockRow, dockCol;
    std::queue<Step> pathSteps;
    int currentBattery;
    int maxBattery;
    bool returningToDock;

    // Pathfinding methods
    std::vector<Step> findPath(int startRow, int startCol, int goalRow, int goalCol);
    int manhattanDistance(int row1, int col1, int row2, int col2) const;
    std::vector<std::pair<int, int>> getNeighbors(int row, int col) const;

    // Utility methods
    std::pair<int, int> findNearestDirt() const;
    void updateMaps();
    bool canSafelyReachTarget(int targetRow, int targetCol) const;
    int estimateRoundTripCost(int targetRow, int targetCol) const;
    void planReturnToDock();

    // Battery management methods
    void updateBattery();
    bool needsRecharge() const;

    // Movement and action methods
    Step executeStep(Step step);
    void clean();

    // Map exploration methods
    void exploreUnknownAreas();
    bool isFullyExplored() const;
};


#endif //VACUUM_FINAL_ALGORITHMASTAR212346076A_H
