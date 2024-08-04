#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <random>
#include <chrono>
#include "hexagon.cpp"


struct Building {
    enum class Type { NONE, SETTLEMENT, CITY };
    Type type;
    int owner;
    Building() : type(Type::NONE), owner(-1) {}  
    Building(Type t, int o) : type(t), owner(o) {}
};

using CornerID = std::pair<int, int>;

struct CornerIDHash {
    std::size_t operator()(const CornerID& k) const {
        return std::hash<int>()(k.first) ^ (std::hash<int>()(k.second) << 1);
    }
};

class CatanBoardManager {
private:
    HexagonGrid& board;
    std::unordered_map<CornerID, Building, CornerIDHash> structures;
    std::unordered_map<CornerID, std::vector<CornerID>, CornerIDHash> corner_graph;
    ResourceManager resourceManager;

    void build_corner_graph() {
        for (const auto& [hex_id, hex] : board.getHexagons()) {
            for (int corner = 0; corner < 6; ++corner) {
                CornerID current = {hex_id, corner};
                auto neighbors = hex->getNeighborConnections();
                
                // Add adjacent corners on the same hexagon
                corner_graph[current].push_back({hex_id, (corner + 1) % 6});
                corner_graph[current].push_back({hex_id, (corner + 5) % 6});
                
                // Add corners from neighboring hexagons
                for (const auto& [neighbor_id, connection] : neighbors) {
                    auto [shared_side, corner1, corner2] = connection;
                    if (corner == corner1 || corner == corner2) {
                        corner_graph[current].push_back({neighbor_id, corner == corner1 ? corner2 : corner1});
                    }
                }
            }
        }
    }

    bool is_corner_free(const CornerID& corner) {
        return structures.find(corner) == structures.end();
    }

    bool check_distance_rule(const CornerID& start_corner) {
        std::queue<std::pair<CornerID, int>> to_visit;
        std::unordered_map<CornerID, bool, CornerIDHash> visited;
        
        to_visit.push({start_corner, 0});
        visited[start_corner] = true;
        
        while (!to_visit.empty()) {
            auto [current, distance] = to_visit.front();
            to_visit.pop();
            
            if (distance > 0 && !is_corner_free(current)) {
                return false;
            }
            
            if (distance >= 2) continue;
            
            for (const auto& neighbor : corner_graph[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    to_visit.push({neighbor, distance + 1});
                }
            }
        }
        
        return true;
    }

public:
    CatanBoardManager(HexagonGrid g, int numPlayers) : board(g), resourceManager(numPlayers) {
    build_corner_graph();
}

    bool place_structure(int hex_id, int corner, Building::Type type, int player) {
        CornerID target = {hex_id, corner};
        
        if (!board.getHexagon(hex_id)) {
            std::cout << "Whoops! That hex is non existent.\n";
            return false;
        }
        
        if (corner < 0 || corner > 5) {
            std::cout << "Corner numbers are like dice - they go from 0 to 5!\n";
            return false;
        }
        
        if (!is_corner_free(target)) {
            std::cout << "This corner is occupied\n";
            return false;
        }
        
        if (!check_distance_rule(target)) {
            std::cout << "Too close to other building\n";
            return false;
        }
        
        structures[target] = Building(type, player);
        board.getHexagon(hex_id)->setCornerLabel(corner, type == Building::Type::SETTLEMENT ? 'S' : 'C');
        
        std::cout << "Structure built!\n";
        return true;
    }

    void show_board() {
        BoardPrinter printer(board);
        printer.printBoard();
    }

    void distributeResources(int roll, ResourceManager& resourceManager) {
        for (const auto& [hex_id, hex] : board.getHexagons()) {
            if (hex->getDiceNumber() == roll) {
                ResourceType resourceType = hex->getResourceType();
                for (int corner = 0; corner < 6; ++corner) {
                    CornerID cornerID = {hex_id, corner};
                    if (structures.find(cornerID) != structures.end()) {
                        const Building& building = structures[cornerID];
                        int resourceAmount = (building.type == Building::Type::CITY) ? 2 : 1;
                        resourceManager.addResource(building.owner, resourceType, resourceAmount);
                    }
                }
            }
        }
    }
};

// int main() {
//     HexagonGrid catan_board;
    
//     std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
//     std::uniform_int_distribution<> dist('A', 'Z');

//     for (int i = 0; i < 19; ++i) {
//         catan_board.addHexagon(static_cast<char>(dist(rng)));
//     }

//     CatanBoardManager game(catan_board);

//     while (true) {
//         game.show_board();

//         int hex, corner, player;
//         std::string building;

//         std::cout << "Hex number (or -1 to bail): ";
//         std::cin >> hex;
//         if (hex == -1) break;

//         std::cout << "Corner (0-5): ";
//         std::cin >> corner;

//         std::cout << "What're we building? (settlement/city): ";
//         std::cin >> building;

//         std::cout << "Which player's staking their claim? ";
//         std::cin >> player;

//         Building::Type type = (building == "city") ? Building::Type::CITY : Building::Type::SETTLEMENT;

//         game.place_structure(hex, corner, type, player);
//     }

//     return 0;
// }