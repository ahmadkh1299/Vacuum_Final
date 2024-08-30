//
// Created by Mariam on 8/12/2024.
//

#ifndef VACUUM_FINAL_SENSORIMPL_H
#define VACUUM_FINAL_SENSORIMPL_H

#include "WallSensor.h"
#include "DirtSensor.h"
#include "BatteryMeter.h"
#include "House.h"
#include "enums.h"

class SensorImpl : public WallsSensor, public DirtSensor, public BatteryMeter {
public:
    SensorImpl(const House& house, int maxBattery);

    ~SensorImpl() = default;

    // WallsSensor interface
    bool isWall(Direction d) const override;

    // DirtSensor interface
    int dirtLevel() const override;

    // BatteryMeter interface
    std::size_t getBatteryState() const override;

    std::pair<int,int> getCurrentPosition() const;

    // Methods to update sensor state
    void updatePosition(Step stepDirection);
    void useBattery();
    std::size_t getMaxBattery() const;
    void chargeBattery();

private:
    const House& house;
    int currentRow;
    int currentCol;
    float batteryLevel;
    int maxBattery;
};

#endif //VACUUM_FINAL_SENSORIMPL_H