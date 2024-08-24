#include "Algorithm_212346076_207177197_B.h"
#include "AlgorithmRegistration.h"

REGISTER_ALGORITHM(Algorithm_212346076_207177197_B);
const Position Algorithm_212346076_207177197_B::DOCK_POS = {0, 0};

Algorithm_212346076_207177197_B::Algorithm_212346076_207177197_B()
        : sensors_(nullptr), steps_(0), max_steps_(0), max_battery_(0),
          current_position_(DOCK_POS), current_state_(State::EXPLORE) {
    explorer_.setDirtLevel(current_position_, static_cast<int>(LocType::Dock));
}

void Algorithm_212346076_207177197_B::setMaxSteps(std::size_t maxSteps) {
    max_steps_ = maxSteps;
}

void Algorithm_212346076_207177197_B::setWallsSensor(const WallsSensor& wallsSensor) {
    sensors_ = const_cast<SensorImpl*>(dynamic_cast<const SensorImpl*>(&wallsSensor));
}

void Algorithm_212346076_207177197_B::setDirtSensor(const DirtSensor& dirtSensor) {
    sensors_ = const_cast<SensorImpl*>(dynamic_cast<const SensorImpl*>(&dirtSensor));
}

void Algorithm_212346076_207177197_B::setBatteryMeter(const BatteryMeter& batteryMeter) {
    sensors_ = const_cast<SensorImpl*>(dynamic_cast<const SensorImpl*>(&batteryMeter));
    max_battery_ = sensors_->getMaxBattery();
}

Step Algorithm_212346076_207177197_B::nextStep() {
    if (shouldFinish()) {
        return Step::Finish;
    }

    steps_++;
    updateExplorerInfo();

    if (current_state_ == State::TO_DOCK) {
        return moveAlongPath();
    }

    if (shouldReturnToDock()) {
        initializeReturnToDock();
        return moveAlongPath();
    }

    switch (current_state_) {
        case State::EXPLORE:
            return exploreAndClean();
        case State::CLEANING:
            return cleanCurrentPosition();
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
    if (!explorer_.explored(current_position_)) {
        explorer_.setDirtLevel(current_position_, sensors_->dirtLevel());
        explorer_.setDistance(current_position_, calculateDistanceFromDock());
        explorer_.removeFromUnexplored(current_position_);
    } else {
        explorer_.updateDirtAndClean(current_position_, sensors_->dirtLevel());
    }

    // Update adjacent areas
    for (const auto& dir : {Direction::North, Direction::East, Direction::South, Direction::West}) {
        Position neighbor = PositionUtils::movePosition(current_position_, dir);
        bool isWall = sensors_->isWall(dir);
        explorer_.AdjacentArea(dir, current_position_, isWall);

        // If the neighbor is not explored and not in unexplored, add it to unexplored
        if (!isWall && !explorer_.explored(neighbor) && !explorer_.isAreaUnexplored(neighbor)) {
            explorer_.updateAdjacentArea(dir, current_position_, false);
        }
    }
}
bool Algorithm_212346076_207177197_B::shouldReturnToDock() {
    auto path = explorer_.getShortestPath_A({current_position_.r, current_position_.c}, {DOCK_POS.r, DOCK_POS.c}, false);
    return path.size() + 1 >= sensors_->getBatteryState() ||
           path.size() >= (max_steps_ - steps_) ||
           isHouseClean();
}

void Algorithm_212346076_207177197_B::initializeReturnToDock() {
    current_state_ = State::TO_DOCK;
    path_to_dock_ = explorer_.getShortestPath_A({current_position_.r, current_position_.c}, {DOCK_POS.r, DOCK_POS.c}, false);
}

Step Algorithm_212346076_207177197_B::moveAlongPath() {
    if (path_to_dock_.empty()) {
        if (current_position_ == DOCK_POS) {
            current_state_ = isHouseClean() ? State::FINISH : State::CHARGING;
            return Step::Stay;
        }
        return Step::Stay;
    }

    Direction next_dir = path_to_dock_.top();
    path_to_dock_.pop();
    current_position_ = PositionUtils::movePosition(current_position_, next_dir);
    return Step(next_dir);
}
/*
Step Algorithm_212346076_207177197_B::exploreAndClean() {
    if (current_position_ != DOCK_POS&&sensors_->dirtLevel() > 0) {
        current_state_ = State::CLEANING;
        return cleanCurrentPosition();
    }

    Position next_pos = findNextDirtyOrUnexplored();
    if (next_pos == current_position_) {
        if(current_position_ == DOCK_POS){ return Step::Finish; }
        initializeReturnToDock();
        return moveAlongPath();
    }

    Direction next_dir = PositionUtils::findDirection(current_position_, next_pos);
    current_position_ = next_pos;
    return Step(next_dir);
}
*/
/*Step Algorithm_212346076_207177197_B::exploreAndClean() {
    updateExplorerInfo();

    if (sensors_->dirtLevel() > 0) {
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
        // If we hit a wall, update the explorer and try again
        explorer_.updateAdjacentArea(next_dir, current_position_, true);
        return exploreAndClean();
    }
}*/
Step Algorithm_212346076_207177197_B::cleanCurrentPosition() {
    if (!explorer_.explored(current_position_)) {
        explorer_.setDirtLevel(current_position_, sensors_->dirtLevel());
        explorer_.setDistance(current_position_, calculateDistanceFromDock());
        explorer_.removeFromUnexplored(current_position_);
    }

    if (sensors_->dirtLevel() > 0) {
        explorer_.updateDirtAndClean(current_position_, sensors_->dirtLevel() - 1);
        return Step::Stay;
    }

    current_state_ = State::EXPLORE;
    return exploreAndClean();
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
/*
 * It first checks immediate neighbors for quick wins (unexplored or dirty cells).
If no immediate neighbors are suitable, it searches the entire known area for the nearest dirty cell or unexplored edge.
An "unexplored edge" would be a cell adjacent to the known area but not yet visited.
 */

bool Algorithm_212346076_207177197_B::isHouseClean() {
    return !explorer_.hasMoreDirtyAreas();
}