#include "../simulator//Vacuum.h"
#include <algorithm> // For std::min

Vacuum::Vacuum() {}

Vacuum::~Vacuum() {}

void Vacuum::init(double battery, Position position) {
    curr_battery = max_battery = battery;
    stepsto_charge = 20;
    curr_pos = position;
    this->docking_station = position;
}

double Vacuum::maxBattery() const {
    return max_battery;
}

double Vacuum::battery() const {
    return curr_battery;
}

bool Vacuum::atDockingStation() const {
    return curr_pos == docking_station;
}

void Vacuum::step(Step stepDirection) {
    if (curr_battery > 0) {
        curr_battery--;
        switch (stepDirection) {
            case Step::North:
                curr_pos.r--;
                break;
            case Step::South:
                curr_pos.r++;
                break;
            case Step::East:
                curr_pos.c++;
                break;
            case Step::West:
                curr_pos.c--;
                break;
            case Step::Stay:
                if (atDockingStation()){
                    charge();
                }
                break;
            case Step::Finish:

                break;
            default:
                break;
        }
    }
}

void Vacuum::charge() {
    curr_battery += max_battery / stepsto_charge;
    curr_battery = std::min(curr_battery, max_battery);
}

Position Vacuum::getPosition() const {
    return curr_pos;
}

