//
// Created by Mariam on 8/12/2024.
//

#include "SensorImpl.h"


SensorImpl::SensorImpl(const House& house, int maxBattery)
        : house(house), currentRow(0), currentCol(0), batteryLevel(maxBattery), maxBattery(maxBattery) {
    currentCol = house.getDockingStation().c;
    currentRow = house.getDockingStation().r;
}



bool SensorImpl::isWall(Direction d) const {
    int checkRow = currentRow;
    int checkCol = currentCol;

    switch (d) {
        case Direction::North: checkRow--; break;
        case Direction::East:  checkCol++; break;
        case Direction::South: checkRow++; break;
        case Direction::West:  checkCol--; break;
    }

    return house.isWall(Position({checkRow, checkCol}));
}

int SensorImpl::dirtLevel() const {
    return house.getDirtLevel(Position({currentRow, currentCol}));
}

std::size_t SensorImpl::getBatteryState() const {
    return batteryLevel;
}

void SensorImpl::updatePosition(Step stepDirection) {
    switch (stepDirection) {
        case Step::North: currentRow--; break;
        case Step::East:  currentCol++; break;
        case Step::South: currentRow++; break;
        case Step::West:  currentCol--; break;
        case Step::Stay: break;
        case Step::Finish: break;
    }
}

std::pair<int,int> SensorImpl::getCurrentPosition() const {
    return {currentRow, currentCol};
}

void SensorImpl::useBattery() {
    batteryLevel--;
    std::cout << "Battery level: " << batteryLevel << std::endl;
}

std::size_t SensorImpl::getMaxBattery() const {
    return maxBattery;
}

void SensorImpl::chargeBattery() {
    float amount = maxBattery * 0.05;
    batteryLevel += amount;
    batteryLevel = std::min(batteryLevel, (float)maxBattery);
}