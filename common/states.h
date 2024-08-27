//
// Created by Mariam on 8/13/2024.
//

#ifndef VACUUM_FINAL_STATES_H
#define VACUUM_FINAL_STATES_H
#include <vector>
#include <iostream>

struct Position {
    int r, c;//row index,col index

    // Define operator< for map comparisons
    bool operator<(const Position& other) const {
        if (r != other.r) {
            return r < other.r;
        } else {
            return c < other.c;
        }
    }

    // Define operator> for equality comparisons
    bool operator>(const Position& other) const {
        if (r != other.r) {
            return r > other.r;
        } else {
            return c > other.c;
        }
    }

    // Define operator== for equality comparisons
    bool operator==(const Position& other) const {
        return r == other.r && c == other.c;
    }

    // Define operator!= for inequality comparisons
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

enum class State {
    CHARGING,
    TO_DOCK,
    TO_POS, // backtracking
    FINISH,
    EXPLORE,
    CLEANING
};

std::ostream& operator<<(std::ostream& os, const State& state) {
    switch (state) {
        case State::CHARGING: os << "CHARGING"; break;
        case State::TO_DOCK: os << "TO_DOCK"; break;
        case State::TO_POS: os << "TO_POS"; break;
        case State::FINISH: os << "FINISH"; break;
        case State::EXPLORE: os << "EXPLORE"; break;
        case State::CLEANING: os << "CLEANING"; break;
        default: os << "UNKNOWN_STATE"; break;
    }
    return os;
}

#define MAXIMUM_DIRT 9
enum class LocType {
    Wall = -1,
    Dock = -20,
};


#endif //VACUUM_FINAL_STATES_H
