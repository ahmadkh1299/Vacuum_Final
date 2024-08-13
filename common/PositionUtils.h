//
// Created by 97250 on 8/13/2024.
//

#ifndef VACUUM_FINAL_POSITIONUTILS_H
#define VACUUM_FINAL_POSITIONUTILS_H


#include "../common/states.h"
#include "../common/enums.h"
#include <iostream>
#include <vector>

class PositionUtils {
public:
    static Position toOffset(Direction dir, bool reverse = false);
    static Direction fromOffset(int x, int y);
    static Position movePosition(Position pos, Direction dir);
    static Direction findDirection(Position src, Position dst);
    static std::vector<Direction> getDirectionOrder();
    static Direction getNextDirection(const Direction &dir);

    friend std::ostream &operator<<(std::ostream &out, const State &state);
};



#endif //VACUUM_FINAL_POSITIONUTILS_H
