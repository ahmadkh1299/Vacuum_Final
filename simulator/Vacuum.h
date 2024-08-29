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
    void init(double battery, Position position);

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

    void setBattery(int battery) {curr_battery = battery;}

    bool atDockingStation() const;


private:
    double curr_battery;   // Current battery level
    double max_battery;     // Maximum battery capacity
    int stepsto_charge;    // Number of steps required for a full charge
    Position curr_pos;     // Current position of the vacuum
    Position docking_station; // Position of the docking station
};

#endif // VACUUM_H