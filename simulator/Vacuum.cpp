#include "../include/Vacuum.h"
#include <algorithm> // For std::min

Vacuum::Vacuum() : curr_state(State::WORKING) {}

Vacuum::~Vacuum() {}

void Vacuum::init(double battery, Position position) {
    curr_battery = MaxBattery = battery;
    stepsto_charge = 20;
    curr_pos = position;
    curr_state = State::WORKING;
}

double Vacuum::maxBattery() const {
    return MaxBattery;
}

double Vacuum::battery() const {
    return curr_battery;
}

void Vacuum::step(Step stepDirection) {
    if (curr_battery > 0) {
        curr_battery--;
        curr_pos = curr_pos.next(stepDirection);

        // Update state based on action
        if (stepDirection == Step::Stay && curr_state != State::CHARGING) {
            curr_state = State::CLEANING;
        } else if (curr_state != State::CHARGING) {
            curr_state = State::WORKING;
        }
    } else {
        curr_state = State::TO_DOCK;
        // TODO: handle error scenario
    }
}

void Vacuum::charge() {
    curr_battery += MaxBattery / stepsto_charge;
    curr_battery = std::min(curr_battery, MaxBattery);
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