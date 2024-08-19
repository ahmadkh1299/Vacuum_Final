//
// Created by 97250 on 8/8/2024.
//

#ifndef HW3_SKELETON_ALGORITHMDFS_H
#define HW3_SKELETON_ALGORITHMDFS_H
#include "../simulator/Explorer.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/SensorImpl.h"
#include "../common/states.h"

class AlgorithmDFS : public AbstractAlgorithm {
public:
    AlgorithmDFS();

    virtual ~AlgorithmDFS() = default;

    void setMaxSteps(std::size_t maxSteps) override;

    void setWallsSensor(const WallsSensor &) override;

    void setDirtSensor(const DirtSensor &) override;

    void setBatteryMeter(const BatteryMeter &) override;

    Step nextStep() override;

    bool StateChanged() const;

    State getCurrentState() const;

    void setSensors(SensorImpl &sensors);

private:
    int max_steps_;
    SensorImpl* sensors_;
    Explorer explorer_;
    bool state_changed;
    State curr_state;
    Position docking_station = {0, 0};
    std::pair<int,int> last_dirty_pos_ = {-20, -20};

    int getMinDistanceOfNeighbors(const Position& curr_pos);
    };


#endif //HW3_SKELETON_ALGORITHMDFS_H
