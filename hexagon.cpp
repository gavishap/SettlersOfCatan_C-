#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <algorithm>


class Side {
protected:
    int id;
public:
    Side() : id(generateId()) {}
    virtual std::string display() const = 0;
    virtual ~Side() = default;
    int getId() const { return id; }

private:
    static int generateId() {
        static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
        static std::uniform_int_distribution<> dis(1, 1000000);
        return dis(gen);
    }
};

class HorizontalSide : public Side {
public:
    std::string display() const override {
        return "-------";
    }
};

class SlantSide : public Side {
    bool isLeft;
public:
    SlantSide(bool isLeft) : isLeft(isLeft) {}
    std::string display() const override {
        if (isLeft) {
            return "/";
        } else {
            return "\\  ";
        }
    }
};

class Corner {
    int id;
    char label;
public:
    Corner(char label) : id(generateId()), label(label) {}
    char getLabel() const { return label; }
    void setLabel(char newLabel) { label = newLabel; }
    int getId() const { return id; }

private:
    static int generateId() {
        static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
        static std::uniform_int_distribution<> dis(1, 1000000);
        return dis(gen);
    }
};

class Hexagon : public std::enable_shared_from_this<Hexagon> {
    std::vector<std::shared_ptr<Side>> sides;
    std::map<int, std::shared_ptr<Corner>> corners;
    std::map<int, std::weak_ptr<Hexagon>> neighbors;
    std::pair<int, int> gridPosition;
    int number;

public:
    Hexagon(char startCorner = 'A', std::pair<int, int> position = {-1, -1}, int num = 0) 
        : gridPosition(position), number(num) {
        sides = {
            std::make_shared<HorizontalSide>(),
            std::make_shared<SlantSide>(false),
            std::make_shared<SlantSide>(true),
            std::make_shared<HorizontalSide>(),
            std::make_shared<SlantSide>(false),
            std::make_shared<SlantSide>(true)
        };

        for (int i = 0; i < 6; ++i) {
            corners[i] = std::make_shared<Corner>(static_cast<char>(startCorner + i));
        }
    }

    int getNumber() const { return number; }

    std::pair<int, int> getGridPosition() const {
        return gridPosition;
    }

    void setGridPosition(std::pair<int, int> position) {
        gridPosition = position;
    }

    void display() const {
        displayTopLine();
        displayUpperSlantLines();
        displayUpperMiddleSlantLines();
        displayMiddleLine();
        displayLowerMiddleSlantLines();
        displayLowerSlantLines();
        displayBottomLine();
    }
    char getCornerLabel(int index) const {
        return corners.at(index)->getLabel();
    }

    std::string getSideDisplay(int index) const {
        return sides[index]->display();
    }
    void displayTopLine() const {
        std::cout << "   " << corners.at(0)->getLabel() << sides[0]->display() << corners.at(1)->getLabel() << std::endl;
    }

    void displayUpperSlantLines() const {
        std::cout << "  " << sides[5]->display() << "         " << sides[1]->display() << std::endl;
    }

    void displayUpperMiddleSlantLines() const {
        std::cout << " " << sides[5]->display() << "           " << sides[1]->display() << std::endl;
    }

    void displayMiddleLine() const {
        std::cout << corners.at(5)->getLabel() << "             " << corners.at(2)->getLabel() << std::endl;
    }

    void displayLowerMiddleSlantLines() const {
        std::cout << " " << sides[4]->display() << "         " << sides[2]->display() << std::endl;
    }

    void displayLowerSlantLines() const {
        std::cout << "  " << sides[4]->display() << "       " << sides[2]->display() << std::endl;
    }

    void displayBottomLine() const {
        std::cout << "   " << corners.at(4)->getLabel() << sides[3]->display() << corners.at(3)->getLabel() << std::endl;
    }

    void attachHexagon(int thisSide, std::shared_ptr<Hexagon> otherHexagon, int otherSide) {
        neighbors[thisSide] = otherHexagon;
        otherHexagon->neighbors[otherSide] = shared_from_this();

        int thisCorner1 = thisSide;
        int thisCorner2 = (thisSide+1) % 6;

        int otherCorner1 = otherSide;
        int otherCorner2 = (otherSide+1) % 6;

        otherHexagon->corners[otherCorner1] = corners[thisCorner2];
        otherHexagon->corners[otherCorner2] = corners[thisCorner1];
    }
    void setCornerLabel(int cornerIndex, char label) {
        if (cornerIndex >= 0 && cornerIndex < 6) {
            corners[cornerIndex]->setLabel(label);
        } else {
            throw std::out_of_range("Corner index must be between 0 and 5");
        }
    }

    std::vector<int> getSortedNeighborNumbers() const {
        std::vector<int> neighborNumbers;
         for (const auto& [side, neighbor] : neighbors) {
            if (!neighbor.expired()) {
                neighborNumbers.push_back(neighbor.lock()->getNumber());
            }
        }
        std::sort(neighborNumbers.begin(), neighborNumbers.end());
        return neighborNumbers;
    }

    std::tuple<int, int, int> getSharedElements(int otherHexNumber) const {
        for (const auto& [side, neighbor] : neighbors) {
            if (!neighbor.expired() && neighbor.lock()->getNumber() == otherHexNumber) {
                int corner1 = side;
                int corner2 = (side + 1) % 6;
                return {side, corner1, corner2};
            }
        }
        throw std::runtime_error("Hexagons are not connected");
    }
    std::map<int, std::tuple<int, int, int>> getNeighborConnections() const {
        std::map<int, std::tuple<int, int, int>> connections;
        std::vector<int> neighborNumbers = getSortedNeighborNumbers();
        for (int neighborNumber : neighborNumbers) {
            try {
                auto [sharedSide, sharedCorner1, sharedCorner2] = getSharedElements(neighborNumber);
                connections[neighborNumber] = std::make_tuple(sharedSide, sharedCorner1, sharedCorner2);
            } catch (const std::runtime_error& e) {
                // This shouldn't happen as we're using getSortedNeighborNumbers,
                // but we'll handle it just in case
                std::cerr << "Unexpected error: " << e.what() << std::endl;
            }
        }
        return connections;
    }
};

class HexagonGrid {
private:
    static const int ROWS = 9;
    static const int COLS = 5;
    std::vector<std::vector<int>> grid;
    std::map<int, std::shared_ptr<Hexagon>> hexagons;
    int countOfHexagons = 0;

    // Helper function to get the grid position of a hexagon
    std::pair<int, int> getGridPosition(int hexNumber) const {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (grid[i][j] == hexNumber) {
                    return {i, j};
                }
            }
        }
        throw std::runtime_error("Hexagon not found in grid");
    }

public:
    HexagonGrid() : grid(ROWS, std::vector<int>(COLS, -1)) {
        // Initialize the grid with the specified layout
        int layout[ROWS][COLS] = {
            {-1, -1,  1, -1, -1},
            {-1,  2, -1,  4, -1},
            { 5, -1,  3, -1,  9},
            {-1,  6, -1,  8, -1},
            {10, -1,  7, -1, 14},
            {-1, 11, -1, 13, -1},
            {15, -1, 12, -1, 19},
            {-1, 16, -1, 18, -1},
            {-1, -1, 17, -1, -1}
        };

        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                grid[i][j] = layout[i][j];
            }
        }
    }
    
    const std::map<int, std::shared_ptr<Hexagon>>& getHexagons() const {
        return hexagons;
    }

    void addHexagon(char startCorner) {
        countOfHexagons++;
        std::pair<int, int> position = getGridPosition(countOfHexagons);
        auto newHexagon = std::make_shared<Hexagon>(startCorner, position, countOfHexagons);
        hexagons[countOfHexagons] = newHexagon;
        connectToNeighbors(countOfHexagons);
    }
    
    void connectToNeighbors(int hexNumber) {
        auto [row, col] = getGridPosition(hexNumber);
        
        // Check and connect to neighbors
        std::vector<std::pair<int, int>> directions = {
            {-2, 0},  // Top
            {-1, 1},  // Top-right
            {1, 1},   // Bottom-right
            {2, 0},   // Bottom
            {1, -1},  // Bottom-left
            {-1, -1}  // Top-left
        };

        for (int i = 0; i < 6; ++i) {
            int newRow = row + directions[i].first;
            int newCol = col + directions[i].second;
            
            if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
                int neighborNumber = grid[newRow][newCol];
                if (neighborNumber != -1 && hexagons.count(neighborNumber)) {
                    hexagons[hexNumber]->attachHexagon(i, hexagons[neighborNumber], (i + 3) % 6);
                }
            }
        }
    }

    void displayGrid() const {
        for (const auto& [hexNumber, hexagon] : hexagons) {
            std::cout << "Hexagon " << hexNumber << " at position " 
                      << hexagon->getGridPosition().first << ", " 
                      << hexagon->getGridPosition().second << std::endl;
            hexagon->display();
            std::cout << std::endl;
        }
    }

    std::shared_ptr<Hexagon> getHexagon(int hexNumber) {
        return hexagons[hexNumber];
    }

    void setHexagon(int hexNumber, std::shared_ptr<Hexagon> hexagon) {
        hexagons[hexNumber] = hexagon;
    }
};

class BoardPrinter{

    HexagonGrid grid;
    BoardPrinter(const HexagonGrid& grid):grid(grid){}

    void printBoard(){
        std::ostringstream boardOutput;
        
        // Iterate through all hexagons in the grid
        for (const auto& [hexNumber, hexagon] : grid.getHexagons()) {
            std::map<int, std::tuple<int, int, int>> connections = hexagon->getNeighborConnections();
            // Add hexagon information to the output
            boardOutput << hexagon->getCornerLabel(0) << hexagon->getSideDisplay(0) << hexagon->getCornerLabel(1) << hexNumber << " at position "
                        << hexagon->getGridPosition().first << ", "
                        << hexagon->getGridPosition().second << "\n";
            
            // Add hexagon display to the output
            std::ostringstream hexDisplay;
         //   hexagon->display(hexDisplay);
            boardOutput << hexDisplay.str() << "\n\n";
        }
        
        // Store the final string
        std::string boardString = boardOutput.str();
    }
};

// void testGetSharedElements(HexagonGrid& grid) {
//     // Assuming hexagons 1 and 2 are connected
//     auto hex1 = grid.getHexagon(1);
//     auto hex2 = grid.getHexagon(2);

//     try {
//         auto [side, corner1, corner2] = hex1->getSharedElements(2);
//         std::cout << "Hexagon 1 and 2 share:\n"
//                   << "Side: " << side << "\n"
//                   << "Corners: " << corner1 << " and " << corner2 << "\n";

//         // Verify symmetry
//         auto [side2, corner2_1, corner2_2] = hex2->getSharedElements(1);
//         std::cout << "Hexagon 2 and 1 share:\n"
//                   << "Side: " << side2 << "\n"
//                   << "Corners: " << corner2_1 << " and " << corner2_2 << "\n";

//         // Test with non-connected hexagon
//         try {
//             hex1->getSharedElements(5);  // Assuming 1 and 5 are not connected
//             std::cout << "Error: Should have thrown for non-connected hexagons\n";
//         } catch (const std::runtime_error& e) {
//             std::cout << "Correctly threw exception for non-connected hexagons: " << e.what() << "\n";
//         }
//     } catch (const std::exception& e) {
//         std::cout << "Test failed: " << e.what() << "\n";
//     }
// }

int main() {
    HexagonGrid grid;

    // Add some hexagons to the grid
    grid.addHexagon('A');
    grid.addHexagon('G');
    grid.addHexagon('M');

    // Display the grid
    std::cout << "Initial grid:" << std::endl;
    grid.displayGrid();

   

    //testGetSharedElements(grid);

    return 0;
}