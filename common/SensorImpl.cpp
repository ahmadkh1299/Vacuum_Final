//
// Created by Mariam on 8/12/2024.
//

#include "SensorImpl.h"
#include "SensorImpl.h"

SensorImpl::SensorImpl(const House& house, int maxBattery)
        : house(house), currentRow(0), currentCol(0), batteryLevel(maxBattery), maxBattery(maxBattery) {}

bool SensorImpl::isWall(Direction d) const {
    int checkRow = currentRow;
    int checkCol = currentCol;

    switch (d) {
        case Direction::North: checkRow--; break;
        case Direction::East:  checkCol++; break;
        case Direction::South: checkRow++; break;
        case Direction::West:  checkCol--; break;
    }

    return house.isWall(checkRow, checkCol);
}

int SensorImpl::dirtLevel() const {
    return house.getDirtLevel(currentRow, currentCol);
}

std::size_t SensorImpl::getBatteryState() const {
    return batteryLevel;
}

void SensorImpl::updatePosition(int row, int col) {
    currentRow = row;
    currentCol = col;
}

void SensorImpl::updateBattery(int newBatteryLevel) {
    batteryLevel = newBatteryLevel;
}