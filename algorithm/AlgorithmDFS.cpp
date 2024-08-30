//

// Created by 97250 on 8/8/2024.

//



#include "AlgorithmDFS.h"
#include "AlgorithmRegistration.h"

AlgorithmDFS::AlgorithmDFS() :
        sensors_(nullptr), max_steps_(0), prev_state(State::EXPLORE), curr_state(State::EXPLORE) {
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
    return prev_state != curr_state;
}

State AlgorithmDFS::getCurrentState() const {
    return curr_state;
}

int AlgorithmDFS::getMinDistanceOfNeighbors(const Position& curr_pos) {
    if (curr_pos == docking_station) return 0;
    int minDistance = INT_MAX;
    std::vector<std::pair<int, int>> neighbors = explorer_.getNeighbors({curr_pos.r, curr_pos.c});

    for (const auto& neighborPair : neighbors) {
        Position neighbor = {neighborPair.first, neighborPair.second};
        if (explorer_.explored(neighbor)) {
            int neighborDistance = explorer_.getDistance(neighbor);
            if (neighborDistance+1 < minDistance) {
                minDistance = neighborDistance + 1;
            }
        }
    }

    if (minDistance == INT_MAX) {
        // None of the neighbors are explored. Handle this case accordingly.
        return -1;  // or any other default value
    }
    return minDistance;
}

void AlgorithmDFS::updateExplorerInfo(Position current_position_) {
    if (!explorer_.explored(current_position_)) {
        explorer_.setDirtLevel(current_position_, sensors_->dirtLevel());
        explorer_.setDistance(current_position_, getMinDistanceOfNeighbors(current_position_));
        explorer_.removeFromUnexplored(current_position_);

        // Update adjacent areas for newly explored positions
        for (const auto& dir : {Direction::North, Direction::East, Direction::South, Direction::West}) {
            explorer_.updateAdjacentArea(dir, current_position_, sensors_->isWall(dir));
        }
    } else {
        explorer_.updateDirtAndClean(current_position_, sensors_->dirtLevel());
    }
}

void AlgorithmDFS::updatePosition(Step stepDirection, Position& current_position_) {
    switch (stepDirection) {
        case Step::North: current_position_.r--; break;
        case Step::East:  current_position_.c++; break;
        case Step::South: current_position_.r++; break;
        case Step::West:  current_position_.c--; break;
        case Step::Stay: break;
        case Step::Finish: break;
    }
}
//function state to string

std::string AlgorithmDFS::stateToString(State state) {
    switch (state) {
        case State::EXPLORE: return "EXPLORE";
        case State::TO_DOCK: return "TO_DOCK";
        case State::TO_POS: return "TO_POS";
        case State::CLEANING: return "CLEANING";
        case State::CHARGING: return "CHARGING";
        case State::FINISH: return "FINISH";
        default: return "?";
    }
}
Step AlgorithmDFS::nextStep() {
    std::cout << "curr_state: " << stateToString(curr_state) << std::endl;
    Position curr_pos = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};
    auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(),
                                            {docking_station.r, docking_station.c}, false);
    if(path.size() >= (max_steps_ - steps_counter)){
        curr_state = State::TO_DOCK;
    }
    if(curr_pos==docking_station){
        Position pos = explorer_.getClosestUnexploredArea(curr_pos);
        path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), {pos.c,pos.r},false);
        if((path.size()>= (max_steps_ - steps_counter))){return Step::Finish; }

    }
    Position possible_pos = curr_pos;
    switch (curr_state) {
        case State::EXPLORE: {
            updateExplorerInfo(curr_pos);
            if (sensors_->dirtLevel() > 0) {
                curr_state = State::CLEANING;
                return nextStep();
            }
            if(curr_pos != docking_station) {
                if (explorer_.getDistance(curr_pos) >= sensors_->getBatteryState() - 1){
                    curr_state = State::TO_DOCK;
                    return nextStep();
                }
            }
            for (Direction dir: PositionUtils::getDirectionOrder()) {
                possible_pos = curr_pos;
                updatePosition(Step(dir), possible_pos);
                if (sensors_->isWall(dir) || explorer_.explored(possible_pos)){
                    continue;
                }
                steps_counter++;
                return Step(dir);
            }
            if(!explorer_.areAllAreasExplored()){
                last_dirty_pos_ = {explorer_.getClosestUnexploredArea(curr_pos).r, explorer_.getClosestUnexploredArea(curr_pos).c};
                curr_state = State::TO_POS;
            }else curr_state = State::TO_DOCK;
            return nextStep();
            break;
        }

        case State::TO_DOCK: {
            prev_state = curr_state;
            explorer_.setDistance(curr_pos, getMinDistanceOfNeighbors(curr_pos));
            auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(),
                                                    {docking_station.r, docking_station.c}, false);
            if (!path.empty()) {
                Step next = Step(path.top());
                path.pop();
                steps_counter++;
                return next;
            }
            if (explorer_.isDockingStation(curr_pos)) {
                curr_state = State::CHARGING;
                steps_counter++;
                return Step::Stay;
            }
            break;
        }

        case State::TO_POS: {
            prev_state = curr_state;
            std::stack<Direction> path;
            if (last_dirty_pos_ == std::make_pair(-20, -20)) {
                if (explorer_.explored(curr_pos)) {
                    path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), last_dirty_pos_, true);
                } else{
                    curr_state = State::EXPLORE;
                    return nextStep();
                }
            }else {
                path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), last_dirty_pos_, false);
            }
            if (path.size() >= sensors_->getBatteryState()-2) {
                curr_state = State::TO_DOCK;
                return nextStep();
            }
            if (!path.empty()) {
                Step next = Step(path.top());
                path.pop();
                steps_counter++;
                return next;
            }
            curr_state = (sensors_->dirtLevel() > 0) ? State::CLEANING : State::EXPLORE;
            return nextStep();
            break;
        }

        case State::CLEANING: {
            prev_state = curr_state;
            std::pair<int, int> dock = {docking_station.r, docking_station.c};
            auto path = explorer_.getShortestPath_A(sensors_->getCurrentPosition(), dock, false);
            if (path.size() >= sensors_->getBatteryState() - 2) {
                curr_state = State::TO_DOCK;
                if (sensors_->dirtLevel() > 0) {
                    last_dirty_pos_ = sensors_->getCurrentPosition();
                } else last_dirty_pos_ = {-20, -20};
                Step s = Step(path.top());
                path.pop();
                steps_counter++;
                return s;
            }
            if (sensors_->dirtLevel() == 0) {
                curr_state = State::EXPLORE;
                return nextStep();
            }
            explorer_.updateDirtAndClean(curr_pos, sensors_->dirtLevel());
            steps_counter++;
            return Step::Stay;
        }

        case State::CHARGING: {
            prev_state = curr_state;
            if (sensors_->getBatteryState() == sensors_->getMaxBattery()) {
                curr_state = State::TO_POS;
                return nextStep();
            }
            steps_counter++;
            return Step::Stay;
        }

        case State::FINISH: {
            break;
/*            if (StateChanged()) {
                prev_state = curr_state;
                return Step::Finish;
            } else {
                prev_state = curr_state;
                curr_state = State::EXPLORE;
                return nextStep();
            }*/
        }
    }
    return Step::Stay;  // Default fallback
}



extern "C" {
REGISTER_ALGORITHM(AlgorithmDFS);
}





