#include "Algorithm_212346076_207177197_B.h"
#include "AlgorithmRegistration.h"
#include <queue>
#include <set>


/*
Algorithm_212346076_207177197_B::Algorithm_212346076_207177197_B()
        : sensors_(nullptr), steps_(0), max_steps_(0), max_battery_(0),
          current_position_(DOCK_POS), current_state_(State::EXPLORE) {
    explorer_ = Explorer();

}*/
Algorithm_212346076_207177197_B::Algorithm_212346076_207177197_B()
    : sensors_(nullptr), steps_(0), max_steps_(0), max_battery_(0),
      current_position_(DOCK_POS), current_state_(State::EXPLORE) {
    explorer_ = Explorer();
    // Initialize the explorer with the starting position
    explorer_.setDirtLevel(current_position_, static_cast<int>(LocType::Dock));
    explorer_.removeFromUnexplored(current_position_);
}

void Algorithm_212346076_207177197_B::setSensors(SensorImpl &sensors) {
    sensors_ = &sensors;
    DOCK_POS = {sensors_->getCurrentPosition().first, sensors_->getCurrentPosition().second};
    explorer_.setDirtLevel(current_position_, static_cast<int>(LocType::Dock));
}


void Algorithm_212346076_207177197_B::setMaxSteps(std::size_t maxSteps) {
    max_steps_ = maxSteps;
}

void Algorithm_212346076_207177197_B::setWallsSensor(const WallsSensor& wallsSensor) {
    setSensors(const_cast<SensorImpl&>(dynamic_cast<const SensorImpl&>(wallsSensor)));
}

void Algorithm_212346076_207177197_B::setDirtSensor(const DirtSensor& dirtSensor) {
    setSensors(const_cast<SensorImpl&>(dynamic_cast<const SensorImpl&>(dirtSensor)));
}

void Algorithm_212346076_207177197_B::setBatteryMeter(const BatteryMeter& batteryMeter) {
    setSensors(const_cast<SensorImpl&>(dynamic_cast<const SensorImpl&>(batteryMeter)));
    max_battery_ = sensors_->getMaxBattery();
}

Step Algorithm_212346076_207177197_B::nextStep() {
    if (steps_ == 0) {
        // First step: update explorer and then explore
        updateExplorerInfo();
        return exploreAndClean();
    }

    steps_++;
    updateExplorerInfo();

    if (shouldFinish()) {
        return Step::Finish;
    }

    if (current_state_!=State::TO_DOCK&&shouldReturnToDock()) {
        initializeReturnToDock();
        return moveAlongPath();
    }

    switch (current_state_) {
        case State::EXPLORE:
            return exploreAndClean();
        case State::CLEANING:
            return cleanCurrentPosition();
        case State::TO_DOCK:
            return moveAlongPath();
        case State::CHARGING:
            return handleCharging();
        default:
            return Step::Stay;
    }
}

bool Algorithm_212346076_207177197_B::shouldFinish() {
    return (sensors_->getBatteryState() == 1 && steps_ == 0) ||
           (max_steps_ - steps_ <= 1 && current_position_ == DOCK_POS) ||
           (current_position_ == DOCK_POS && isHouseClean());
}


void Algorithm_212346076_207177197_B::updateExplorerInfo() {
    // Update current position
    explorer_.setDirtLevel(current_position_, sensors_->dirtLevel());
    explorer_.setDistance(current_position_, calculateDistanceFromDock());
    explorer_.removeFromUnexplored(current_position_);

    // Update adjacent areas
    for (const auto& dir : {Direction::North, Direction::East, Direction::South, Direction::West}) {
        bool isWall = sensors_->isWall(dir);
        explorer_.updateAdjacentArea(dir, current_position_, isWall);
    }

    // If we're cleaning, update the dirt level
    if (current_state_ == State::CLEANING) {
        explorer_.updateDirtAndClean(current_position_, sensors_->dirtLevel());
    }
    std::cout << "Updated known area. Mapped: " << explorer_.mapped_areas_.size() 
              << ", Unexplored: " << explorer_.unexplored_areas_.size() << std::endl;
}

/*bool Algorithm_212346076_207177197_B::shouldReturnToDock() {
    auto path = explorer_.getShortestPath_A({current_position_.r, current_position_.c}, {DOCK_POS.r, DOCK_POS.c}, false);
    return path.size() + 5 >= sensors_->getBatteryState() ||
           path.size() >= (max_steps_ - steps_) ||(isHouseClean()&&explorer_.areAllAreasExplored());
           
}*/
bool Algorithm_212346076_207177197_B::shouldReturnToDock() {
    auto path = explorer_.findPathToDock(current_position_, DOCK_POS);
    return path.size() + 10 >= sensors_->getBatteryState() ||
           path.size() + 5 >= (max_steps_ - steps_) ||
           (isHouseClean() && explorer_.areAllAreasExplored());
}

/*void Algorithm_212346076_207177197_B::initializeReturnToDock() {
    current_state_ = State::TO_DOCK;
    path_to_dock_ = explorer_.getShortestPath_A({current_position_.r, current_position_.c}, {DOCK_POS.r, DOCK_POS.c}, false);
}*/
void Algorithm_212346076_207177197_B::initializeReturnToDock() { //bfs
    current_state_ = State::TO_DOCK;
    path_to_dock_ = explorer_.findPathToDock(current_position_, DOCK_POS);
    if (path_to_dock_.empty()) {
        std::cout << "Failed to find path to dock. Current position: (" 
                  << current_position_.r << "," << current_position_.c 
                  << "), Dock position: (" << DOCK_POS.r << "," << DOCK_POS.c << ")" << std::endl;
    }
}

/*Step Algorithm_212346076_207177197_B::moveAlongPath() {
    if (path_to_dock_.empty()&& (current_position_.c==DOCK_POS.c&&current_position_.r==DOCK_POS.r)) {
        if (current_position_ == DOCK_POS) {
            current_state_ = isHouseClean() ? State::FINISH : State::CHARGING;
            return Step::Stay;
        }
        return Step::Stay;
    }

    Direction next_dir = path_to_dock_.top();
    path_to_dock_.pop();
    current_position_ = PositionUtils::movePosition(current_position_, next_dir);
    updateExplorerInfo();
    return Step(next_dir);
}*/
Step Algorithm_212346076_207177197_B::moveAlongPath() {
    if (path_to_dock_.empty()) {
        if (current_position_ == DOCK_POS) {
            current_state_ = isHouseClean() ? State::FINISH : State::CHARGING;
            return Step::Stay;
        }
        // Recalculate path if it's empty
        path_to_dock_ = explorer_.findPathToDock(current_position_, DOCK_POS);
    }

    Direction next_dir = path_to_dock_.top();
    path_to_dock_.pop();
    current_position_ = PositionUtils::movePosition(current_position_, next_dir);
    updateExplorerInfo();
    return Step(next_dir);
}

Step Algorithm_212346076_207177197_B::exploreAndClean() {
    if (sensors_->dirtLevel() > 0 && current_position_ != DOCK_POS) {
        current_state_ = State::CLEANING;
        return cleanCurrentPosition();
    }

    Position next_pos = findNextDirtyOrUnexplored();
    if (next_pos == current_position_) {
        initializeReturnToDock();
        return moveAlongPath();
    }

    Direction next_dir = PositionUtils::findDirection(current_position_, next_pos);
    if (!sensors_->isWall(next_dir)) {
        current_position_ = next_pos;
        return Step(next_dir);
    } else {
        explorer_.updateAdjacentArea(next_dir, current_position_, true);
        return exploreAndClean();
    }
}

Step Algorithm_212346076_207177197_B::cleanCurrentPosition() {
    if (sensors_->dirtLevel() > 0) {
        explorer_.updateDirtAndClean(current_position_, sensors_->dirtLevel() - 1);
        return Step::Stay;
    }

    current_state_ = State::EXPLORE;
    return exploreAndClean();
}

Step Algorithm_212346076_207177197_B::handleCharging() {
    if (sensors_->getBatteryState() == max_battery_) {
        current_state_ = State::EXPLORE;
        return exploreAndClean();
    }
    return Step::Stay;
}

Position Algorithm_212346076_207177197_B::findNextDirtyOrUnexplored() {
    for (const auto& dir : {Direction::North, Direction::East, Direction::South, Direction::West}) {
        Position neighbor = PositionUtils::movePosition(current_position_, dir);
        if (!sensors_->isWall(dir) && (explorer_.isAreaUnexplored(neighbor) || explorer_.getDirtLevel(neighbor) > 0)) {
            return neighbor;
        }
    }

    auto path = explorer_.getShortestPath_A({current_position_.r, current_position_.c}, {-1, -1}, true);
    return path.empty() ? current_position_ : PositionUtils::movePosition(current_position_, path.top());
}
/*Position Algorithm_212346076_207177197_B::findNextDirtyOrUnexplored() {
    std::queue<Position> q;
    std::set<Position> visited;
    q.push(current_position_);
    visited.insert(current_position_);

    while (!q.empty()) {
        Position current = q.front();
        q.pop();

        // Check if this position is dirty or unexplored
        if (explorer_.getDirtLevel(current) > 0 || explorer_.isAreaUnexplored(current)) {
            return current;
        }

        // Add neighboring positions to the queue
        for (const auto& dir : {Direction::North, Direction::East, Direction::South, Direction::West}) {
            Position neighbor = PositionUtils::movePosition(current, dir);
            
            // Check if the neighbor is valid and not visited
            if (!explorer_.isWall(neighbor) && visited.find(neighbor) == visited.end()) {
                q.push(neighbor);
                visited.insert(neighbor);
            }
        }
    }

    // If no dirty or unexplored area found, return the current position
    return current_position_;
}
*/
bool Algorithm_212346076_207177197_B::isHouseClean() { //it is better to move this method to the explorer class
    return !explorer_.hasMoreDirtyAreas()&&explorer_.areAllAreasExplored();
}

int Algorithm_212346076_207177197_B::calculateDistanceFromDock() {
    auto path = explorer_.getShortestPath_A({current_position_.r, current_position_.c}, {DOCK_POS.r, DOCK_POS.c}, false);
    return path.size();
}
extern "C" {
    REGISTER_ALGORITHM(Algorithm_212346076_207177197_B);
}
