#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include "catanBoardManager.cpp"


class Player {
public:
    std::string name;
    int points;

    Player(std::string n) : name(n), points(0) {}
};

class Game {
private:
    std::vector<Player> players;
    CatanBoardManager boardManager;
    ResourceManager resourceManager;
    int currentPlayerIndex;

public:
    Game(int numPlayers) : currentPlayerIndex(0), resourceManager(numPlayers), boardManager(HexagonGrid(), numPlayers) {
}

    void addPlayer(const std::string& name) {
        players.emplace_back(name);
    }

    void start() {
        std::cout << "Starting the game!" << std::endl;
        boardManager.show_board();
    }

    void playTurn() {
        Player& currentPlayer = players[currentPlayerIndex];
        std::cout << "\n" << currentPlayer.name << "'s turn" << std::endl;

        int roll = rollDice();
        std::cout << currentPlayer.name << " rolled a " << roll << std::endl;

        // Distribute resources based on roll (this would need to be implemented in CatanBoardManager)
        // boardManager.distributeResources(roll, resourceManager);

        resourceManager.printPlayerResources(currentPlayerIndex);

        bool turnEnded = false;
        while (!turnEnded) {
            std::cout << "\nWhat would you like to do?" << std::endl;
            std::cout << "1. Build a road" << std::endl;
            std::cout << "2. Build a settlement" << std::endl;
            std::cout << "3. Build a city" << std::endl;
            std::cout << "4. Buy a development card" << std::endl;
            std::cout << "5. Trade with bank" << std::endl;
            std::cout << "6. Trade with player" << std::endl;
            std::cout << "7. End turn" << std::endl;

            int choice;
            std::cin >> choice;

            switch(choice) {
                case 1:
                    buildRoad(currentPlayerIndex);
                    break;
                case 2:
                    buildSettlement(currentPlayerIndex);
                    break;
                case 3:
                    buildCity(currentPlayerIndex);
                    break;
                case 4:
                    buyDevelopmentCard(currentPlayerIndex);
                    break;
                case 5:
                    tradeWithBank(currentPlayerIndex);
                    break;
                case 6:
                    tradeWithPlayer(currentPlayerIndex);
                    break;
                case 7:
                    turnEnded = true;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
            }
        }

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

    void buildRoad(int playerIndex) {
        // Check if player has resources
        if (resourceManager.hasResources(playerIndex, {ResourceType::WOOD, ResourceType::BRICK})) {
            // Implement road building logic
            std::cout << "Building a road..." << std::endl;
            // Remove resources
            resourceManager.removeResources(playerIndex, {ResourceType::WOOD, ResourceType::BRICK});
        } else {
            std::cout << "Not enough resources to build a road." << std::endl;
        }
    }

    void buildSettlement(int playerIndex) {
        if (resourceManager.hasResources(playerIndex, {ResourceType::WOOD, ResourceType::BRICK, ResourceType::WOOL, ResourceType::GRAIN})) {
            std::cout << "Building a settlement..." << std::endl;
            // Implement settlement building logic with boardManager
            int hex_id, corner;
            std::cout << "Enter hex number: ";
            std::cin >> hex_id;
            std::cout << "Enter corner (0-5): ";
            std::cin >> corner;

            if (boardManager.place_structure(hex_id, corner, Building::Type::SETTLEMENT, playerIndex)) {
                std::cout << "Settlement built successfully!" << std::endl;
                players[playerIndex].points++;
            } else {
                std::cout << "Failed to build settlement. Make sure the location is valid and follows game rules." << std::endl;
                // Refund resources since building failed
                resourceManager.addResources(playerIndex, {ResourceType::WOOD, ResourceType::BRICK, ResourceType::WOOL, ResourceType::GRAIN});
            }
            // Remove resources
            resourceManager.removeResources(playerIndex, {ResourceType::WOOD, ResourceType::BRICK, ResourceType::WOOL, ResourceType::GRAIN});
            players[playerIndex].points++;
        } else {
            std::cout << "Not enough resources to build a settlement." << std::endl;
        }
    }

    void buildCity(int playerIndex) {
        if (resourceManager.hasResources(playerIndex, {ResourceType::ORE, ResourceType::ORE, ResourceType::ORE, ResourceType::GRAIN, ResourceType::GRAIN})) {
            std::cout << "Building a city..." << std::endl;
            // Implement city building logic with boardManager
            int hex_id, corner;
            std::cout << "Enter hex number: ";
            std::cin >> hex_id;
            std::cout << "Enter corner (0-5): ";
            std::cin >> corner;

            if (boardManager.place_structure(hex_id, corner, Building::Type::CITY, playerIndex)) {
                std::cout << "City built successfully!" << std::endl;
                players[playerIndex].points++;
            } else {
                std::cout << "Failed to build city. Make sure the location is valid and follows game rules." << std::endl;
                // Refund resources since building failed
                resourceManager.addResources(playerIndex, {ResourceType::ORE, ResourceType::ORE, ResourceType::ORE, ResourceType::GRAIN, ResourceType::GRAIN});
            }
            // Remove resources
            resourceManager.removeResources(playerIndex, {ResourceType::ORE, ResourceType::ORE, ResourceType::ORE, ResourceType::GRAIN, ResourceType::GRAIN});
            players[playerIndex].points++;
        } else {
            std::cout << "Not enough resources to build a city." << std::endl;
        }
    }

    void buyDevelopmentCard(int playerIndex) {
        if (resourceManager.hasResources(playerIndex, {ResourceType::WOOL, ResourceType::GRAIN, ResourceType::ORE})) {
            std::cout << "Buying a development card..." << std::endl;
            // Implement development card logic
            // Remove resources
            resourceManager.removeResources(playerIndex, {ResourceType::WOOL, ResourceType::GRAIN, ResourceType::ORE});
        } else {
            std::cout << "Not enough resources to buy a development card." << std::endl;
        }
    }

    void tradeWithBank(int playerIndex) {
        // Implement bank trading logic
        std::cout << "Trading with bank..." << std::endl;
    }

    void tradeWithPlayer(int playerIndex) {
        // Implement player trading logic
        std::cout << "Trading with player..." << std::endl;
    }
};

int main() {
    Game game(3);  // Start a game with 3 players

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