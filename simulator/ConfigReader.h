#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <algorithm>
#include <iostream>

class ConfigReader {
public:
    ConfigReader(const std::string &file_path) {
        readFromFile(file_path);
        parseConfig();
    }

    const std::vector<std::string>& getLayout() const {
        return layout;
    }

    int getMaxSteps() const {
        return max_steps;
    }

    int getMaxBattery() const {
        return max_battery;
    }

    int getRows() const {
        return rows;
    }

    int getCols() const {
        return cols;
    }

private:
    std::vector<std::string> layout;
    int max_steps;
    int max_battery;
    int rows;
    int cols;

    void readFromFile(const std::string &file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        std::string line;
        while (std::getline(file, line)) {
            layout.push_back(line);
        }
        file.close();
    }

    void parseConfig() {
        if (layout.size() < 6) {
            throw std::runtime_error("Invalid file: Not enough lines");
        }

        max_steps = parseValue("MaxSteps", layout[1]);
        max_battery = parseValue("MaxBattery", layout[2]);
        rows = parseValue("Rows", layout[3]);
        cols = parseValue("Cols", layout[4]);

        // Validate and extract the house layout
        extractHouseLayout();

        // Validate docking station and other requirements
        validateDockingStation();
    }

    int parseValue(const std::string &key, const std::string &line) {
        std::regex re(key + "\\s*=\\s*(\\d+)");
        std::smatch match;
        if (std::regex_search(line, match, re)) {
            return std::stoi(match[1]);
        } else {
            throw std::runtime_error("Invalid file: Missing or malformed " + key);
        }
    }

    void extractHouseLayout() {
        std::vector<std::string> house_layout;
        for (int i = 5; i < layout.size() && i < 5 + rows; ++i) {
            house_layout.push_back(layout[i].substr(0, cols));
        }

        // Fill in missing rows with '0'
        while (house_layout.size() < rows) {
            house_layout.push_back(std::string(cols, '0'));
        }

        // Fill in missing columns with '0'
        for (auto &row : house_layout) {
            if (row.size() < cols) {
                row.append(cols - row.size(), '0');
            }
        }

        // Enclose the house layout with walls
        for (auto& row : house_layout) {
            row.insert(row.begin(), 'W');
            row.push_back('W');
        }
        std::string wall_row(cols + 2, 'W');
        house_layout.insert(house_layout.begin(), wall_row);
        house_layout.push_back(wall_row);

        layout = house_layout;
    }

    void validateDockingStation() {
        int docking_station_count = 0;
        for (const auto &row : layout) {
            docking_station_count += std::count(row.begin(), row.end(), 'D');
        }
        if (docking_station_count != 1) {
            throw std::runtime_error("Invalid file: There must be exactly one docking station 'D'");
        }
    }
};

#endif // CONFIG_READER_H