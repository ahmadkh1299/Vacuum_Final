//

// Created by 97250 on 8/8/2024.

//



#include "AlgorithmDFS.h"

#include "AlgorithmRegistration.h"





AlgorithmDFS::AlgorithmDFS() :

sensors_(nullptr), max_steps_(0), state_changed(false), curr_state(State::EXPLORE) {

    explorer_ = Explorer();

}



void AlgorithmDFS::setSensors(SensorImpl &sensors) {

    sensors_ = &sensors;

    docking_station = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};

    explorer_.setDirtLevel(docking_station, static_cast<int>(LocType::Dock));

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



int AlgorithmDFS::getMinDistanceOfNeighbors(const Position& curr_pos) {

    int minDistance = INT_MAX;



    std::vector<std::pair<int, int>> neighbors = explorer_.getNeighbors({curr_pos.r, curr_pos.c});

    for (const auto& neighborPair : neighbors) {

        Position neighbor = {neighborPair.first, neighborPair.second};

        if (explorer_.explored(neighbor)) {

            int neighborDistance = explorer_.getDistance(neighbor);

            if (neighborDistance < minDistance) {

                minDistance = neighborDistance;

            }

        }

    }



    if (minDistance == INT_MAX) {

        // None of the neighbors are explored. Handle this case accordingly.

        return -1;  // or any other default value

    }



    return minDistance + 1;

}





Step AlgorithmDFS::nextStep() {

    Position curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};

    switch (curr_state) {

        case State::EXPLORE: {

            for (Direction dir : PositionUtils::getDirectionOrder()) {

                sensors_->updatePosition(Step(dir));

                curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};

                if (!explorer_.explored(curr_pos) && !sensors_->isWall(dir)) {

                    explorer_.setDirtLevel(curr_pos, sensors_->dirtLevel());

                    explorer_.setDistance(curr_pos, getMinDistanceOfNeighbors(curr_pos));

                    if (sensors_->dirtLevel() > 0) {

                        state_changed = true;

                        curr_state = State::CLEANING;

                    } else state_changed = false;

                    return Step(dir);

                }

            }

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

                curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};

                explorer_.setDistance(curr_pos, getMinDistanceOfNeighbors(curr_pos));

                if (explorer_.isDockingStation(curr_pos)){

                    state_changed = true;

                    curr_state = explorer_.hasMoreDirtyAreas() ? State::CHARGING : State::FINISH;

                } else state_changed = false;

                return next;

            }

            break;

        }



        case State::TO_POS: {

            if(last_dirty_pos_ == std::make_pair(-20,-20)){

                state_changed = true;

                curr_state = State::EXPLORE;

                return nextStep();

            }

            auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), last_dirty_pos_, true);

            if (!path.empty()) {

                Step next = Step(path.top());

                path.pop();

                sensors_->updatePosition(next);

                curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};

                explorer_.setDistance(curr_pos, getMinDistanceOfNeighbors(curr_pos));

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

                if (sensors_->dirtLevel() > 0) {

                    last_dirty_pos_ = sensors_->getCurrentPosition();

                } else last_dirty_pos_ = {-20,-20};

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

                curr_state = State::TO_POS;

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



extern "C" {

    REGISTER_ALGORITHM(AlgorithmDFS);

}






