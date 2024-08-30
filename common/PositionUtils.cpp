//
// Created by 97250 on 8/13/2024.
//

#include "PositionUtils.h"

Position PositionUtils::toOffset(Direction dir, bool reverse) {
    int factor = reverse ? -1 : 1;
    switch (dir) {
        case Direction::North:
            return {factor * -1, 0};
        case Direction::South:
            return {factor * 1, 0};
        case Direction::East:
            return {0, factor * 1};
        case Direction::West:
            return {0, factor * -1};
        default:
            return {0, 0};
    }
}

Direction PositionUtils::fromOffset(int x, int y) {
    if (x == 1)
        return Direction::South;
    if (x == -1)
        return Direction::North;
    if (y == 1)
        return Direction::East;
    if (y == -1)
        return Direction::West;

    // Default error value
    return Direction::North;
}

Position PositionUtils::movePosition(Position pos, Direction dir) {
    Position offset = toOffset(dir);
    return {pos.r + offset.r, pos.c + offset.c};
}

Direction PositionUtils::findDirection(Position src, Position dst) {
    Position difference = {dst.r - src.r, dst.c - src.c};
    if ((std::abs(difference.r) + std::abs(difference.c)) != 1) {
        std::cerr << __FUNCTION__ << " ERROR!! Invalid parameters in findDirection" << std::endl;
    }
    return fromOffset(difference.r, difference.c);
}

std::vector<Direction> PositionUtils::getDirectionOrder() {
    return {Direction::North, Direction::East, Direction::South, Direction::West};
}

Direction PositionUtils::getNextDirection(const Direction &dir) {
    auto directions = getDirectionOrder();
    int i = 0;
    while (directions[i] != dir) {
        i++;
    }
    return directions[(i + 1) % 4];
}

std::ostream &operator<<(std::ostream &out, const State &state) {
    switch (state) {
        case State::CHARGING:
            out << "CHARGING";
            break;
        case State::TO_DOCK:
            out << "TO_DOCK";
            break;
        case State::TO_POS:
            out << "TO_POS";
            break;
        case State::FINISH:
            out << "FINISH";
            break;
        case State::EXPLORE:
            out << "EXPLORE";
            break;
        case State::CLEANING:
            out << "CLEANING";
            break;
        default:
            out << "UNKNOWN_STATE";
    }
    return out;
}

