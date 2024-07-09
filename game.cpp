#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

class Resource {
public:
    enum Type { Wood, Brick, Ore, Grain, Wool };
    static std::string toString(Type type) {
        switch (type) {
            case Wood: return "Wood";
            case Brick: return "Brick";
            case Ore: return "Ore";
            case Grain: return "Grain";
            case Wool: return "Wool";
            default: return "Unknown";
        }
    }
};

class Tile {
public:
    Resource::Type resource;
    int number;
    Tile(Resource::Type r, int n) : resource(r), number(n) {}
};

class Player {
public:
    std::string name;
    std::vector<Resource::Type> resources;
    int points;

    Player(std::string n) : name(n), points(0) {}

    void addResource(Resource::Type resource) {
        resources.push_back(resource);
    }

    void printResources() {
        std::cout << name << "'s resources: ";
        for (const auto& resource : resources) {
            std::cout << Resource::toString(resource) << " ";
        }
        std::cout << std::endl;
    }
};



class Game {
private:
    std::vector<Player> players;
    Board board;
    int currentPlayerIndex;

public:
    Game() : currentPlayerIndex(0) {}

    void addPlayer(const std::string& name) {
        players.emplace_back(name);
    }

    void start() {
        std::cout << "Starting the game!" << std::endl;
        board.print();
    }

    void playTurn() {
        Player& currentPlayer = players[currentPlayerIndex];
        std::cout << "\n" << currentPlayer.name << "'s turn" << std::endl;

        // Roll dice
        int roll = rollDice();
        std::cout << currentPlayer.name << " rolled a " << roll << std::endl;

        // Distribute resources based on roll
        for (const auto& tile : board.tiles) {
            if (tile.number == roll) {
                currentPlayer.addResource(tile.resource);
                std::cout << currentPlayer.name << " gets " << Resource::toString(tile.resource) << std::endl;
            }
        }

        currentPlayer.printResources();

        // Simple building logic (just for demonstration)
        if (currentPlayer.resources.size() >= 4) {
            std::cout << currentPlayer.name << " builds a settlement and gains a point!" << std::endl;
            currentPlayer.points++;
            currentPlayer.resources.erase(currentPlayer.resources.begin(), currentPlayer.resources.begin() + 4);
        }

        std::cout << currentPlayer.name << " now has " << currentPlayer.points << " points" << std::endl;

        // Move to next player
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    }

    bool isGameOver() {
        for (const auto& player : players) {
            if (player.points >= 10) {
                std::cout << player.name << " wins the game!" << std::endl;
                return true;
            }
        }
        return false;
    }

private:
    int rollDice() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(2, 12);
        return dis(gen);
    }
};

int main() {
    Game game;

    // Add players
    game.addPlayer("Player 1");
    game.addPlayer("Player 2");
    game.addPlayer("Player 3");

    game.start();

    while (!game.isGameOver()) {
        game.playTurn();

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    return 0;
}