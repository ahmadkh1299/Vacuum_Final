#ifndef ALGORITHM_212346076_207177197_B_H
#define ALGORITHM_212346076_207177197_B_H

#include "../common/AbstractAlgorithm.h"
#include "../common/SensorImpl.h"
#include "../simulator/Explorer.h"
#include "../common/PositionUtils.h"
#include <stack>

class Algorithm_212346076_207177197_B : public AbstractAlgorithm {
public:
    Algorithm_212346076_207177197_B();
    ~Algorithm_212346076_207177197_B() = default;

    void setMaxSteps(std::size_t maxSteps) override;
    void setWallsSensor(const WallsSensor&) override;
    void setDirtSensor(const DirtSensor&) override;
    void setBatteryMeter(const BatteryMeter&) override;
    Step nextStep() override;

    void setSensors(SensorImpl &sensors);

private:
    SensorImpl* sensors_;
    Explorer explorer_;
    std::size_t steps_;
    std::size_t max_steps_;
    std::size_t max_battery_;
    Position current_position_;
    State current_state_;
    std::stack<Direction> path_to_dock_;

    Position DOCK_POS = {0, 0};

    bool shouldFinish();
    void updateExplorerInfo();
    bool shouldReturnToDock();
    void initializeReturnToDock();
    Step moveAlongPath();
    Step exploreAndClean();
    Step cleanCurrentPosition();
    Step handleCharging();
    Position findNextDirtyOrUnexplored();
    bool isHouseClean();
    int calculateDistanceFromDock();
};

#endif // ALGORITHM_212346076_207177197_B_H