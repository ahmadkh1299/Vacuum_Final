#include "Vacuum.h"
#include <algorithm>

Vacuum::Vacuum(House& house, int max_steps, int max_battery)
        : house(house),
          row(house.getDockingStationRow()),
          col(house.getDockingStationCol()),
          steps_taken(0),
          battery_left(max_battery),
          max_steps(max_steps),
          max_battery(max_battery),
          in_dock(true),
          finished(false) {
}

bool Vacuum::move(Step step) {
    if (isDead() || isFinished()) {
        return false;
    }

    int new_row = row;
    int new_col = col;

    switch (step) {
        case Step::North: new_row--; break;
        case Step::East:  new_col++; break;
        case Step::South: new_row++; break;
        case Step::West:  new_col--; break;
        case Step::Stay:  break;
        case Step::Finish:
            if (in_dock) {
                finished = true;
                steps_taken++;
                return true;
            }
            return false;
    }

    if (house.isValidPosition(new_row, new_col)) {
        row = new_row;
        col = new_col;
        steps_taken++;
        battery_left--;

        if (house.getDirtLevel(row, col) > 0) {
            house.cleanCell(row, col);
        }

        in_dock = house.isInDock(row, col);
        if (in_dock) {
            int charge_amount = std::min(max_battery / 20, max_battery - battery_left);
            battery_left += charge_amount;
        }

        return true;
    }

    return false;
}

bool Vacuum::isFinished() const {
    return finished || steps_taken >= max_steps || (house.isClean() && in_dock);
}

bool Vacuum::isDead() const {
    return battery_left <= 0;
}

int Vacuum::getScore() const {
    int dirt_left = house.getTotalDirt();

    if (isDead()) {
        return max_steps + dirt_left * 300 + 2000;
    } else if (finished && !in_dock) {
        return max_steps + dirt_left * 300 + 3000;
    } else {
        return steps_taken + dirt_left * 300 + (in_dock ? 0 : 1000);
    }
}

int Vacuum::getStepsTaken() const {
    return steps_taken;
}

bool Vacuum::isInDock() const {
    return in_dock;
}

bool Vacuum::isWall(Direction d) const {
    int check_row = row;
    int check_col = col;

    switch (d) {
        case Direction::North: check_row--; break;
        case Direction::East:  check_col++; break;
        case Direction::South: check_row++; break;
        case Direction::West:  check_col--; break;
    }

    return house.isWall(check_row, check_col);
}

int Vacuum::dirtLevel() const {
    return house.getDirtLevel(row, col);
}

std::size_t Vacuum::getBatteryState() const {
    return battery_left;
}