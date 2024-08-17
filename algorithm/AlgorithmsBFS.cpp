//
// Created by 97250 on 8/8/2024.
//

#include "AlgorithmsBFS.h"
#include "AlgorithmRegistration.h"

REGISTER_ALGORITHM(AlgorithmsBFS);

AlgorithmsBFS::AlgorithmsBFS() :
        explorer_(), sensors_(nullptr) {}

void AlgorithmsBFS::setSensors(const SensorImpl &sensors) {
    sensors_ = &sensors;
}

void AlgorithmsBFS::setMaxSteps(std::size_t maxSteps) {
    max_steps_ = maxSteps;
}

void AlgorithmsBFS::setWallsSensor(const WallsSensor &wallsSensor) {
    sensors_ = dynamic_cast<const SensorImpl*>(&wallsSensor);
}

void AlgorithmsBFS::setDirtSensor(const DirtSensor &dirtSensor) {
    sensors_ = dynamic_cast<const SensorImpl*>(&dirtSensor);
}

void AlgorithmsBFS::setBatteryMeter(const BatteryMeter &batteryMeter) {
    sensors_ = dynamic_cast<const SensorImpl*>(&batteryMeter);
}

Step AlgorithmsBFS::nextStep() {
    // Your algorithm implementation
    return Step();
}
