#include "../simulator//Vacuum.h"
#include <algorithm> // For std::min

Vacuum::Vacuum() : curr_state(State::WORKING) {}

Vacuum::~Vacuum() {}

void Vacuum::init(double battery, Position position, Position docking_station) {
    curr_battery = max_battery = battery;
    stepsto_charge = 20;
    curr_pos = position;
    curr_state = State::WORKING;
    this->docking_station = docking_station;
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
                curr_state = State::FINISH;
                break;
            default:
                break;
        }
    }
}

void Vacuum::charge() {
    curr_battery += max_battery / stepsto_charge;
    curr_battery = std::min(curr_battery, max_battery);
    curr_state = State::CHARGING;
}

Position Vacuum::getPosition() const {
    return curr_pos;
}

State Vacuum::getState() const {
    return curr_state;
}

void Vacuum::setState(State newState) {
    curr_state = newState;
}