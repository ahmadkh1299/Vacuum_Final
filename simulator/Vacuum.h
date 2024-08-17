#ifndef VACUUM_H
#define VACUUM_H

#include "../common/states.h" // Includes Position and Step definitions
#include "../common/enums.h" // Includes Position and Step definitions

class Vacuum {
public:
    // Default constructor
    Vacuum();

    // Destructor
    ~Vacuum();

    // Initializes the vacuum with a given battery level and starting position
    void init(double battery, Position position, Position docking_station);

    // Returns the maximum battery capacity
    double maxBattery() const;

    // Returns the current battery level
    double battery() const;

    // Moves the vacuum in the specified direction, consuming battery
    void step(Step stepDirection);

    // Charges the vacuum's battery
    void charge();

    // Returns the current position of the vacuum
    Position getPosition() const;

    // Returns the current state of the vacuum
    State getState() const;

    // Sets the state of the vacuum
    void setState(State newState);

    bool atDockingStation() const;


private:
    State curr_state;      // Current state of the vacuum
    double curr_battery;   // Current battery level
    double max_battery;     // Maximum battery capacity
    int stepsto_charge;    // Number of steps required for a full charge
    Position curr_pos;     // Current position of the vacuum
    Position docking_station; // Position of the docking station
};

#endif // VACUUM_H