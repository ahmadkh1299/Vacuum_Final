//
// Created by 97250 on 8/8/2024.
//

#include "AlgorithmDFS.h"
#include "AlgorithmRegistration.h"

REGISTER_ALGORITHM(AlgorithmDFS);

AlgorithmDFS::AlgorithmDFS() :
sensors_(nullptr) {
    explorer_ = Explorer();
}

void AlgorithmDFS::setSensors(SensorImpl &sensors) {
    sensors_ = &sensors;
}

void AlgorithmDFS::setMaxSteps(std::size_t maxSteps) {
    max_steps_ = maxSteps;
}


void AlgorithmDFS::setWallsSensor(const WallsSensor& wallsSensor) {
    sensors_ = const_cast<SensorImpl*>(dynamic_cast<const SensorImpl*>(&wallsSensor));
}

void AlgorithmDFS::setDirtSensor(const DirtSensor& dirtSensor) {
    sensors_ = const_cast<SensorImpl*>(dynamic_cast<const SensorImpl*>(&dirtSensor));
}

void AlgorithmDFS::setBatteryMeter(const BatteryMeter& batteryMeter) {
    sensors_ = const_cast<SensorImpl*>(dynamic_cast<const SensorImpl*>(&batteryMeter));
}


Step AlgorithmDFS::nextStep() {
    Step step = Step();
    sensors_->updatePosition(step);
    return step;
}




// Your algorithm implementation

