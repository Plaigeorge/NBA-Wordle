#ifndef NBAWORDLE_GAMELOGIC_H
#define NBAWORDLE_GAMELOGIC_H

#include <string>
#include <vector>

struct Player {
    std::string name;
    std::string team;
    std::string conference;
    std::string position;
    std::string height;
    std::string age;
    std::string number;

    // Default constructor
    Player() {}

    // Parameterized constructor
    Player(std::string name, std::string team, std::string conference, std::string position,
           std::string height, std::string age, std::string number)
            : name(std::move(name)), team(std::move(team)), conference(std::move(conference)), position(std::move(position)),
              height(std::move(height)), age(std::move(age)), number(std::move(number)) {}
};

std::vector<Player> parseAndStorePlayerData();
std::string checkAttributes(const Player &guessedPlayer, const Player &actualPlayer, std::stringstream& ss);
void checkGuess(const std::string& guess, const std::vector<Player>& players, const Player& targetPlayer);

#endif // NBAWORDLE_GAMELOGIC_H
