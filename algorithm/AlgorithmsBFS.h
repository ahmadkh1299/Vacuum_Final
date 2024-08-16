//
// Created by 97250 on 8/8/2024.
//

#ifndef HW3_SKELETON_ALGORITHMSBFS_H
#define HW3_SKELETON_ALGORITHMSBFS_H
#include "../simulator/Explorer.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/SensorImpl.h"


class AlgorithmsBFS : public AbstractAlgorithm {
    AlgorithmsBFS();

    ~AlgorithmsBFS() = default;

    void setMaxSteps(std::size_t maxSteps) override;

    void setWallsSensor(const WallsSensor &) override;

    void setDirtSensor(const DirtSensor &) override;

    void setBatteryMeter(const BatteryMeter &) override;

    Step nextStep() override;

private:
    int max_steps_;
    SensorImpl sensors_;
    Explorer explorer_;
};



#endif //HW3_SKELETON_ALGORITHMSBFS_H
