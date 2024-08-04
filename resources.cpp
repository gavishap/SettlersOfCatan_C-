#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

enum class ResourceType {
    WOOD,
    BRICK,
    ORE,
    GRAIN,
    WOOL,
    DESERT,
    NONE
};

class ResourceDeck {
private:
    std::map<ResourceType, int> resources;

public:
    ResourceDeck() {
        resources[ResourceType::WOOD] = 19;
        resources[ResourceType::BRICK] = 19;
        resources[ResourceType::ORE] = 19;
        resources[ResourceType::GRAIN] = 19;
        resources[ResourceType::WOOL] = 19;
    }

    bool takeResource(ResourceType type) {
        if (resources[type] > 0) {
            resources[type]--;
            return true;
        }
        return false;
    }

    void returnResource(ResourceType type) {
        resources[type]++;
    }

    int getResourceCount(ResourceType type) const {
        return resources.at(type);
    }
};

class PlayerHand {
private:
    std::map<ResourceType, int> resources;

public:
    void addResource(ResourceType type) {
        resources[type]++;
    }

    bool removeResource(ResourceType type) {
        if (resources[type] > 0) {
            resources[type]--;
            return true;
        }
        return false;
    }

    int getResourceCount(ResourceType type) const {
        return resources.count(type) ? resources.at(type) : 0;
    }

    int getTotalResourceCount() const {
        int total = 0;
        for (const auto& [type, count] : resources) {
            total += count;
        }
        return total;
    }

    bool hasResources(const std::vector<ResourceType>& requiredResources) const {
        for (const auto& type : requiredResources) {
            if (getResourceCount(type) == 0) {
                return false;
            }
        }
        return true;
    }

    void removeResources(const std::vector<ResourceType>& resourcesToRemove) {
        for (const auto& type : resourcesToRemove) {
            removeResource(type);
        }
    }

    void addResources(const std::vector<ResourceType>& resourcesToAdd) {
        for (const auto& type : resourcesToAdd) {
            addResource(type);
        }
    }
};

class ResourceManager {
private:
    ResourceDeck deck;
    std::vector<PlayerHand> playerHands;

public:
    ResourceManager(int numPlayers) : playerHands(numPlayers) {}

    bool distributeResource(int playerIndex, ResourceType type) {
        if (deck.takeResource(type)) {
            playerHands[playerIndex].addResource(type);
            return true;
        }
        return false;
    }

    bool tradeWithBank(int playerIndex, ResourceType giveType, ResourceType receiveType) {
        if (playerHands[playerIndex].getResourceCount(giveType) >= 4) {
            for (int i = 0; i < 4; i++) {
                playerHands[playerIndex].removeResource(giveType);
                deck.returnResource(giveType);
            }
            if (deck.takeResource(receiveType)) {
                playerHands[playerIndex].addResource(receiveType);
                return true;
            }
        }
        return false;
    }

    bool tradeWithPlayer(int fromPlayer, int toPlayer, ResourceType fromType, ResourceType toType) {
        if (playerHands[fromPlayer].removeResource(fromType) && 
            playerHands[toPlayer].removeResource(toType)) {
            playerHands[fromPlayer].addResource(toType);
            playerHands[toPlayer].addResource(fromType);
            return true;
        }
        return false;
    }

    void printPlayerResources(int playerIndex) const {
        std::cout << "Player " << playerIndex << " resources:\n";
        for (const auto& type : {ResourceType::WOOD, ResourceType::BRICK, ResourceType::ORE, ResourceType::GRAIN, ResourceType::WOOL}) {
            std::cout << static_cast<int>(type) << ": " << playerHands[playerIndex].getResourceCount(type) << "\n";
        }
    }

    void addResource(int playerIndex, ResourceType type, int amount) {
        for (int i = 0; i < amount; i++) {
            if (deck.takeResource(type)) {
                playerHands[playerIndex].addResource(type);
            }
        }
    }

    bool hasResources(int playerIndex, const std::vector<ResourceType>& requiredResources) {
        return playerHands[playerIndex].hasResources(requiredResources);
    }

    void removeResources(int playerIndex, const std::vector<ResourceType>& resourcesToRemove) {
        playerHands[playerIndex].removeResources(resourcesToRemove);
    }

    void addResources(int playerIndex, const std::vector<ResourceType>& resourcesToAdd) {
        playerHands[playerIndex].addResources(resourcesToAdd);
    }
};