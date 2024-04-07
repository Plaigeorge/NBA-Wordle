#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip> // For std::setw
#include "GameLogic.h"
#include <iostream>
#include <fstream>

// Function to extract a value given a line and a key
std::string extractValue(std::string& line, const std::string& key) {
    size_t startPos = line.find(key);
    if (startPos == std::string::npos) return ""; // Key not found
    startPos += key.length() + 3; // Adjust to get the position after "key":"
    size_t endPos = line.find("\"", startPos);
    return line.substr(startPos, endPos - startPos);
}

// Function to parse player data and store it in a vector of Player structures
std::vector<Player> parseAndStorePlayerData() {
    std::vector<Player> players;
    std::ifstream file("common_player_info3.json");
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return players; // Return empty vector if file couldn't be opened
    }

    while (getline(file, line)) {
        // Assuming each player's block starts with "{" and ends with "}"
        if (line.find("{") != std::string::npos) {
            std::string name, team, conference, position, height, age, number;

            while (getline(file, line) && line.find("}") == std::string::npos) {
                if (line.find("\"display_first_last\":") != std::string::npos) {
                    name = extractValue(line, "\"display_first_last\"");
                } else if (line.find("\"team_name\":") != std::string::npos) {
                    team = extractValue(line, "\"team_name\"");
                } else if (line.find("\"Conference\":") != std::string::npos) {
                    conference = extractValue(line, "\"Conference\"");
                } else if (line.find("\"position\":") != std::string::npos) {
                    position = extractValue(line, "\"position\"");
                } else if (line.find("\"height\":") != std::string::npos) {
                    height = extractValue(line, "\"height\"");
                } else if (line.find("\"Age\":") != std::string::npos) {
                    age = extractValue(line, "\"Age\"");
                } else if (line.find("\"jersey\":") != std::string::npos) {
                    number = extractValue(line, "\"jersey\"");
                }
            }

            // Create a Player object and add it to the vector
            players.emplace_back(name, team, conference, position, height, age, number);
        }
    }

    file.close();
    return players;
}
std::string checkAttributes(const Player &guessedPlayer, const Player &actualPlayer, std::stringstream& ss)
 {


    ss << std::left << std::setw(15) << "Attribute"
       << std::setw(25) << "Guessed"
       << "Result" << std::endl;

    // Team comparison
    ss << std::setw(15) << "Team" << std::setw(25) << guessedPlayer.team
       << (guessedPlayer.team == actualPlayer.team ? "Green (Match)" : "Gray (No match)") << std::endl;

    // Conference comparison
    ss << std::setw(15) << "Conference" << std::setw(25) << guessedPlayer.conference
       << (guessedPlayer.conference == actualPlayer.conference ? "Green (Match)" : "Gray (No match)") << std::endl;

    // Position comparison
    std::string positionResult = "Gray (No match)";
    if (guessedPlayer.position == actualPlayer.position) {
        positionResult = "Green (Match)";
    } else {
        // Considering hybrid positions
        std::vector<std::string> hybridPositions = {"Forward-Center", "Center-Forward", "Guard-Forward", "Forward-Guard"};
        bool guessedHybrid = std::find(hybridPositions.begin(), hybridPositions.end(), guessedPlayer.position) != hybridPositions.end();
        bool actualHybrid = std::find(hybridPositions.begin(), hybridPositions.end(), actualPlayer.position) != hybridPositions.end();
        if (guessedHybrid || actualHybrid) {
            positionResult = "Yellow (Partial match)";
        }
    }
    ss << std::setw(15) << "Position" << std::setw(25) << guessedPlayer.position << positionResult << std::endl;

    // Height comparison
    std::string heightResult = "Gray (No match)";
    if (guessedPlayer.height == actualPlayer.height) {
        heightResult = "Green (Match)";
    } else {
        // Simple string comparison; you might want to improve this to handle numeric comparison
        heightResult = "Yellow (Close match)";
    }
    ss << std::setw(15) << "Height" << std::setw(25) << guessedPlayer.height << heightResult << std::endl;

    // Age comparison
    std::string ageResult = "Gray (No match)";
    if (guessedPlayer.age == actualPlayer.age) {
        ageResult = "Green (Match)";
    } else {
        // Simple string comparison; consider parsing and comparing integers for closer age comparison
        ageResult = "Yellow (Close match)";
    }
    ss << std::setw(15) << "Age" << std::setw(25) << guessedPlayer.age << ageResult << std::endl;

    // Jersey Number comparison
    std::string jerseyResult = "Gray (No match)";
    if (guessedPlayer.number == actualPlayer.number) {
        jerseyResult = "Green (Match)";
    } else {
        // Simple string comparison; consider parsing and comparing integers for closer number comparison
        jerseyResult = "Yellow (Close match)";
    }
    ss << std::setw(15) << "Jersey Number" << std::setw(25) << guessedPlayer.number << jerseyResult << std::endl;

    return ss.str();
}

// In main.cpp
void checkGuess(const std::string& guess, const std::vector<Player>& players, const Player& targetPlayer) {
    for (const auto& player : players) {
        if (guess == player.name) {
            std::cout << "You guessed: " << player.name << std::endl;
            std::stringstream ss;
            checkAttributes(player, targetPlayer, ss);
            std::cout << ss.str() << std::endl; // Correctly display the output
            return;
        }
    }
    std::cout << "No matching player found." << std::endl;
}
