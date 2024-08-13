//
// Created by Mariam on 8/13/2024.
//

#ifndef VACUUM_FINAL_STATES_H
#define VACUUM_FINAL_STATES_H
#include <vector>
#include <iostream>

struct Position {
    int r, c;//row index,col index
};

enum class State {
    CHARGING,
    TO_DOCK,
    TO_POS,
    WORKING,
    FINISH,
    EXPLORE,
    CLEANING

};


#endif //VACUUM_FINAL_STATES_H
