#ifndef HW3_SKELETON_ALGORITHM_212346076_207177197_B_H
#define HW3_SKELETON_ALGORITHM_212346076_207177197_B_H

#include "../simulator/Explorer.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/SensorImpl.h"
#include "../common/states.h"
#include <climits>
#include <iostream>
#include <queue>

class Algorithm_212346076_207177197_B : public AbstractAlgorithm {
public:
    Algorithm_212346076_207177197_B();
    virtual ~Algorithm_212346076_207177197_B() = default;
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
    int steps_counter = 0;
    SensorImpl* sensors_;
    std::queue<Position> bfs_queue;
    Explorer explorer_;
    State prev_state;
    State curr_state;
    Position docking_station = {0, 0};
    std::pair<int,int> last_dirty_pos_ = {-20, -20};

    void updateExplorerInfo(Position current_position_);
    int getMinDistanceOfNeighbors(const Position& curr_pos);
    void updatePosition(Step stepDirection, Position& curr_pos);
    std::string stateToString(State state);
};

#endif //HW3_SKELETON_ALGORITHM_212346076_207177197_B_H