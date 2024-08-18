//
// Created by 97250 on 8/8/2024.
//

#include "AlgorithmDFS.h"
#include "AlgorithmRegistration.h"

REGISTER_ALGORITHM(AlgorithmDFS);

AlgorithmDFS::AlgorithmDFS() :
sensors_(nullptr), max_steps_(0), state_changed(false), curr_state(State::EXPLORE) {
    explorer_ = Explorer();
}

void AlgorithmDFS::setSensors(SensorImpl &sensors) {
    sensors_ = &sensors;
    docking_station = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};
}

void AlgorithmDFS::setMaxSteps(std::size_t maxSteps) {
    max_steps_ = maxSteps;
}


void AlgorithmDFS::setWallsSensor(const WallsSensor& wallsSensor) {
    setSensors(const_cast<SensorImpl&>(dynamic_cast<const SensorImpl&>(wallsSensor)));
}

void AlgorithmDFS::setDirtSensor(const DirtSensor& dirtSensor) {
    setSensors(const_cast<SensorImpl&>(dynamic_cast<const SensorImpl&>(dirtSensor)));
}

void AlgorithmDFS::setBatteryMeter(const BatteryMeter& batteryMeter) {
    setSensors(const_cast<SensorImpl&>(dynamic_cast<const SensorImpl&>(batteryMeter)));
}

bool AlgorithmDFS::StateChanged() const {
    return state_changed;
}

State AlgorithmDFS::getCurrentState() const {
    return curr_state;
}


Step AlgorithmDFS::nextStep() {
    Position curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};
    switch (curr_state) {
        case State::EXPLORE: {
            // DFS-type exploration logic within AlgorithmDFS
            for (Direction dir : PositionUtils::getDirectionOrder()) {
                sensors_->updatePosition(Step(dir));
                curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};
                if (!explorer_.explored(curr_pos) && !sensors_->isWall(dir)) {
                    explorer_.setDirtLevel(curr_pos, sensors_->dirtLevel());
                    if (sensors_->dirtLevel() > 0) {
                        state_changed = true;
                        curr_state = State::CLEANING;
                    } else state_changed = false;
                    return Step(dir);
                }
            }

            // If no unexplored direction, check if we need to return to dock
            if (explorer_.getDistance(curr_pos) >= sensors_->getBatteryState() || !explorer_.hasMoreDirtyAreas()) {
                state_changed = true;
                curr_state = State::TO_DOCK;
            } else {
                state_changed = false;
            }
            break;
        }

        case State::TO_DOCK: {
            auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), {docking_station.r,docking_station.c}, false);
            if (!path.empty()) {
                Step next = Step(path.top());
                path.pop();
                sensors_->updatePosition(next);
                if (explorer_.isDockingStation(curr_pos)){
                    state_changed = true;
                    curr_state = explorer_.hasMoreDirtyAreas() ? State::CHARGING : State::FINISH;
                } else state_changed = false;
                return next;
            }
            break;
        }

        case State::TO_POS: {
            auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), last_dirty_pos_, true);
            if (!path.empty()) {
                Step next = Step(path.top());
                path.pop();
                sensors_->updatePosition(next);
                if (sensors_->getCurrentPosition() == last_dirty_pos_) {
                    curr_state = (sensors_->dirtLevel() > 0) ? State::CLEANING : State::EXPLORE;
                }
                return next;
            }
            break;
        }
        case State::CLEANING: {
            std::pair<int,int> dock = {docking_station.r, docking_station.c};
            auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), dock, false);
            if (path.size() == sensors_->getBatteryState()-2){
                state_changed = true;
                curr_state = State::TO_DOCK;
                Step s = Step(path.top());
                path.pop();
                return s;
            }
            if (sensors_->dirtLevel() == 0) {
                state_changed = true;
                curr_state = State::EXPLORE;
                return nextStep();
            }
            state_changed = false;
            explorer_.updateDirtAndClean(curr_pos, sensors_->dirtLevel()-1);
            return Step::Stay;
        }
        case State::CHARGING: {
            if (sensors_->getBatteryState() == sensors_->getMaxBattery()) {
                state_changed = true;
                curr_state = State::EXPLORE;
                return nextStep();
            } else state_changed = false;
            return Step::Stay;
        }
        case State::FINISH: {
            return Step::Finish;
        }
    }

    return Step::Stay;  // Default fallback
}





// Your algorithm implementation

