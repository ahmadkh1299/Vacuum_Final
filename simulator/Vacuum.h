#ifndef VACUUM_H
#define VACUUM_H

#include "../common/enums.h"
#include "House.h"

class Vacuum {
public:
    Vacuum(House& house, int max_steps, int max_battery);

    // Movement and state methods
    bool move(Step step);
    bool isFinished() const;
    bool isDead() const;
    int getScore() const;
    int getStepsTaken() const;
    bool isInDock() const;

    // Methods to get sensor information (now using House directly)
    bool isWall(Direction d) const;
    int dirtLevel() const;
    std::size_t getBatteryState() const;

private:
    House& house;
    int row, col;
    int steps_taken;
    int battery_left;
    int max_steps;
    int max_battery;
    bool in_dock;
    bool finished;

    void updateScore();
};


#endif //HW3_SKELETON_VACUUM_H
