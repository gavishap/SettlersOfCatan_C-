#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <cassert>
#include "resources.cpp"

// Abstract class for partial sides of a hexagon:
class SlantPartialSide {
protected:
    int id;
    std::string color;
public:
    SlantPartialSide() : id(generateId()), color("white") {}
    virtual std::string display() const = 0;
    virtual ~SlantPartialSide() = default;
    int getId() const { return id; }
    void setColor(const std::string& newColor) { color = newColor; }
    std::string getColor() const { return color; }

private:
    static int generateId() {
        static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
        static std::uniform_int_distribution<> dis(1, 1000000);
        return dis(gen);
    }
};

class HorizontalPartialSide : public SlantPartialSide {
public:
    std::string display() const override {
        return "-------";
    }
};

class SlantSide : public SlantPartialSide {
    bool isLeft;
public:
    SlantSide(bool isLeft) : isLeft(isLeft) {}
    std::string display() const override {
        return isLeft ? "/" : "\\";
    }
};

// New HexSide class
class HexSide {
public:
    std::shared_ptr<SlantPartialSide> first;
    std::shared_ptr<SlantPartialSide> second;

    HexSide() : first(nullptr), second(nullptr) {}  
    HexSide(std::shared_ptr<SlantPartialSide> f, std::shared_ptr<SlantPartialSide> s = nullptr)
        : first(f), second(s) {}

    std::string display() const {
        std::string result = first->display();
        if (second) {
            result += second->display();
        }
        return result;
    }

    void setColor(const std::string& newColor) {
        first->setColor(newColor);
        if (second) {
            second->setColor(newColor);
        }
    }

    std::string getColor() const {
        return first->getColor();
    }
};

class Corner {
    int id;
    char label;
    bool isOccupied;
public:
    Corner(char label) : id(generateId()), label(label), isOccupied(false) {}
    char getLabel() const { return label; }
    void setLabel(char newLabel) { label = newLabel; }
    int getId() const { return id; }
    bool getIsOccupied() const { return isOccupied; }
    void setIsOccupied(bool occupied) { isOccupied = occupied; }

private:
    static int generateId() {
        static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
        static std::uniform_int_distribution<> dis(1, 1000000);
        return dis(gen);
    }
};

class Hexagon : public std::enable_shared_from_this<Hexagon> {
    std::array<HexSide, 6> sides;
    std::array<std::shared_ptr<Corner>, 6> corners;
    std::array<std::weak_ptr<Hexagon>, 6> neighbors;
    std::pair<int, int> gridPosition;
    ResourceType resourceType;
    int diceNumber;
    int number;

public:
    Hexagon(char startCorner = 'A', std::pair<int, int> position = {-1, -1}, int num = 0, ResourceType resource = ResourceType::NONE, int dice = 0) 
    : gridPosition(position), number(num), resourceType(resource), diceNumber(dice) {
        sides[0] = HexSide(std::make_shared<HorizontalPartialSide>());
        sides[1] = HexSide(std::make_shared<SlantSide>(false), std::make_shared<SlantSide>(false));
        sides[2] = HexSide(std::make_shared<SlantSide>(false), std::make_shared<SlantSide>(false));
        sides[3] = HexSide(std::make_shared<HorizontalPartialSide>());
        sides[4] = HexSide(std::make_shared<SlantSide>(true), std::make_shared<SlantSide>(true));
        sides[5] = HexSide(std::make_shared<SlantSide>(true), std::make_shared<SlantSide>(true));

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
    const std::array<HexSide, 6>& getSides() const {
    return sides;
}
    ResourceType getResourceType() const {
        return resourceType;
    }

    void setResourceType(ResourceType type) {
        resourceType = type;
    }

    int getDiceNumber() const {
        return diceNumber;
    }

    void setDiceNumber(int num) {
        diceNumber = num;
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
        return sides[index].display();
    }

    void displayTopLine() const {
        std::cout << "   " << corners[0]->getLabel() << sides[0].display() << corners[1]->getLabel() << std::endl;
    }

    void displayUpperSlantLines() const {
        std::cout << "  " << sides[5].display() << "         " << sides[1].display() << std::endl;
    }

    void displayUpperMiddleSlantLines() const {
        std::cout << " " << sides[5].display() << "           " << sides[1].display() << std::endl;
    }

    void displayMiddleLine() const {
        std::cout << corners[5]->getLabel() << "             " << corners[2]->getLabel() << std::endl;
    }

    void displayLowerMiddleSlantLines() const {
        std::cout << " " << sides[4].display() << "         " << sides[2].display() << std::endl;
    }

    void displayLowerSlantLines() const {
        std::cout << "  " << sides[4].display() << "       " << sides[2].display() << std::endl;
    }

    void displayBottomLine() const {
        std::cout << "   " << corners[4]->getLabel() << sides[3].display() << corners[3]->getLabel() << std::endl;
    }

    void attachHexagon(int thisSide, std::shared_ptr<Hexagon> otherHexagon, int otherSide) {
        if (thisSide < 0 || thisSide >= 6 || otherSide < 0 || otherSide >= 6) {
            std::cerr << "Invalid side index in attachHexagon" << std::endl;
            return;
        }
        
        if (neighbors[thisSide].lock() || otherHexagon->neighbors[otherSide].lock()) {
            std::cerr << "One or both sides are already attached" << std::endl;
            return;
        }
        
        neighbors[thisSide] = otherHexagon;
        otherHexagon->neighbors[otherSide] = shared_from_this();

        std::cout << "Attached Hexagon " << getNumber() << " side " << thisSide 
                  << " to Hexagon " << otherHexagon->getNumber() << " side " << otherSide << std::endl;

        // Share sides
        otherHexagon->sides[otherSide] = sides[thisSide];

        // Ensure colors are shared
        otherHexagon->sides[otherSide].setColor(sides[thisSide].getColor());

        // Connect corners
        int thisCorner1 = thisSide;
        int thisCorner2 = (thisSide + 1) % 6;
        int otherCorner1 = otherSide;
        int otherCorner2 = (otherSide + 1) % 6;

        otherHexagon->corners[otherCorner1] = corners[thisCorner2];
        otherHexagon->corners[otherCorner2] = corners[thisCorner1];

        std::cout << "Connected corners: " << thisCorner2 << " -> " << otherCorner1 
                  << " and " << thisCorner1 << " -> " << otherCorner2 << std::endl;
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
        for (const auto& neighborWeak : neighbors) {
            if (auto neighbor = neighborWeak.lock()) {
                neighborNumbers.push_back(neighbor->getNumber());
            }
        }
        std::sort(neighborNumbers.begin(), neighborNumbers.end());
        return neighborNumbers;
    }

    std::tuple<int, int, int> getSharedElements(int otherHexNumber) const {
        for (int i = 0; i < 6; ++i) {
            if (auto neighbor = neighbors[i].lock()) {
                if (neighbor->getNumber() == otherHexNumber) {
                    int corner1 = i;
                    int corner2 = (i + 1) % 6;
                    return {i, corner1, corner2};
                }
            }
        }
        throw std::runtime_error("Hexagons are not connected");
    }

    std::map<int, std::tuple<int, int, int>> getNeighborConnections() const {
        std::map<int, std::tuple<int, int, int>> connections;
        for (int i = 0; i < 6; ++i) {
            if (auto neighbor = neighbors[i].lock()) {
                connections[neighbor->getNumber()] = std::make_tuple(i, i, (i + 1) % 6);
                
                std::cout << "Checking shared sides for hexagon " << getNumber() 
                          << " and " << neighbor->getNumber() << std::endl;
                std::cout << "sharedSide: " << i << ", otherSide: " << (i + 3) % 6 << std::endl;
                
                if (sides[i].first != neighbor->sides[(i + 3) % 6].first) {
                    std::cerr << "Shared sides are not properly linked for hexagon " 
                              << getNumber() << " and " << neighbor->getNumber() << std::endl;
                } else {
                    std::cout << "Shared sides are correctly linked" << std::endl;
                }
            }
        }
        return connections;
    }

    void updateSideColor(int sideIndex, const std::string& color) {
        if (sideIndex >= 0 && sideIndex < 6) {
            sides[sideIndex].setColor(color);
        } else {
            throw std::out_of_range("Side index must be between 0 and 5");
        }
    }

    std::string getSideColoredDisplay(int sideIndex) const {
        if (sideIndex >= 0 && sideIndex < 6) {
            const auto& side = sides[sideIndex];
            std::string display;
            std::string color = side.getColor();
            std::string colorCode;
            
            if (color == "red") colorCode = "\033[31m";
            else if (color == "blue") colorCode = "\033[34m";
            else if (color == "green") colorCode = "\033[32m";
            else if (color == "yellow") colorCode = "\033[33m";
            else colorCode = "\033[0m"; // Default color

            display += colorCode + side.first->display() + "\033[0m";
            if (side.second) {
                display += colorCode + side.second->display() + "\033[0m";
            }
            return display;
        }
        return ""; // Return empty string for invalid indices
    }

    void verifyConnections() const {
        for (int i = 0; i < 6; ++i) {
            if (auto neighbor = neighbors[i].lock()) {
                int otherSide = (i + 3) % 6;
                if (neighbor->neighbors[otherSide].lock().get() != this) {
                    std::cerr << "Inconsistent connection between Hexagon " << getNumber()
                              << " side " << i << " and Hexagon " << neighbor->getNumber()
                              << " side " << otherSide << std::endl;
                }
                if (sides[i].first != neighbor->sides[otherSide].first) {
                    std::cerr << "Shared sides are not properly linked for hexagon " 
                              << getNumber() << " and " << neighbor->getNumber() << std::endl;
                }
            }
        }
    }
};

std::string getResourceName(ResourceType type) {
    static const std::map<ResourceType, std::string> resourceNames = {
        {ResourceType::WOOD, "Wood"},
        {ResourceType::BRICK, "Brick"},
        {ResourceType::ORE, "Ore"},
        {ResourceType::GRAIN, "Grain"},
        {ResourceType::WOOL, "Wool"},
        {ResourceType::DESERT, "Desert"},
        {ResourceType::NONE, "None"}
    };
    return resourceNames.at(type);
}

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

    void addHexagon(char startCorner, ResourceType resourceType, int diceNumber) {
        countOfHexagons++;
        std::pair<int, int> position = getGridPosition(countOfHexagons);
        auto newHexagon = std::make_shared<Hexagon>(startCorner, position, countOfHexagons, resourceType, diceNumber);
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
                      << hexagon->getGridPosition().second 
                      << " Resource: " << getResourceName(hexagon->getResourceType())
                      << " Dice Number: " << hexagon->getDiceNumber() << std::endl;
            hexagon->display();
            std::cout << std::endl;
        }
    }

    std::shared_ptr<Hexagon> getHexagon(int hexNumber) const {
        auto it = hexagons.find(hexNumber);
        if (it != hexagons.end()) {
            return it->second;
        }
        return nullptr;
    }

    void setHexagon(int hexNumber, std::shared_ptr<Hexagon> hexagon) {
        hexagons[hexNumber] = hexagon;
    }

    void verifyAllConnections() const {
        for (const auto& [_, hexagon] : hexagons) {
            hexagon->verifyConnections();
        }
    }

    void printBoard() const {
        std::stringstream boardOutput;

        // Add 25 spaces to the beginning of the boardOutput
        boardOutput << std::string(25, ' ');
        // Add the first hexagon's top line to the boardOutput
        if (!hexagons.empty()) {
            //First line
            auto firstHexagon = hexagons.begin()->second;

            boardOutput << firstHexagon->getCornerLabel(0)
                        << firstHexagon->getSideColoredDisplay(0)
                        << firstHexagon->getCornerLabel(1)
                        << "\n";
            //Second and Third line
            boardOutput << std::string(24, ' ');
            
            boardOutput << firstHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << firstHexagon->getSideColoredDisplay(1) << "\n";
        
            boardOutput << std::string(23, ' ');
        
            boardOutput << firstHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << firstHexagon->getSideColoredDisplay(2) << "\n";

            //Fourth line
            boardOutput << std::string(14, ' ');
            
            auto secondHexagon = getHexagon(2);
            boardOutput << secondHexagon->getCornerLabel(0) << secondHexagon->getSideColoredDisplay(0)<< secondHexagon->getCornerLabel(1);
            auto fourthHexagon = getHexagon(4);
            boardOutput << std::string(13, ' ') << fourthHexagon->getCornerLabel(0) <<fourthHexagon->getSideColoredDisplay(0)<< fourthHexagon->getCornerLabel(1)<< "\n";

            //Fifth line
            boardOutput << std::string(13, ' ');
            boardOutput << secondHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << secondHexagon->getSideColoredDisplay(1) ;
            boardOutput << std::string(9, ' ')<< fourthHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << fourthHexagon->getSideColoredDisplay(1) << "\n";

            //Sixth line
            boardOutput << std::string(12, ' ');
            boardOutput << secondHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << secondHexagon->getSideColoredDisplay(2) << std::string(7, ' ')<< fourthHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << fourthHexagon->getSideColoredDisplay(2) << "\n";

            //Seventh Line
            auto thirdHexagon = getHexagon(3);
            auto fifthHexagon = getHexagon(5);
            auto ninthHexagon = getHexagon(9);

            boardOutput << std::string(3, ' ')<<fifthHexagon->getCornerLabel(0)<<fifthHexagon->getSideColoredDisplay(0);
            boardOutput << fifthHexagon->getCornerLabel(1) << std::string(13, ' ') << secondHexagon->getCornerLabel(2) 
            << thirdHexagon->getSideColoredDisplay(0)<< thirdHexagon->getCornerLabel(1) << std::string(13, ' ') << ninthHexagon->getCornerLabel(0) << ninthHexagon->getSideColoredDisplay(0) << ninthHexagon->getCornerLabel(1) << "\n";

            //Eighth line
            boardOutput << std::string(2, ' ');
            boardOutput << fifthHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << fifthHexagon->getSideColoredDisplay(1) << std::string(9, ' ') << thirdHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << fourthHexagon->getSideColoredDisplay(4) <<std::string(9, ' ')<< ninthHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << ninthHexagon->getSideColoredDisplay(1) << std::string(9, ' ') <<  "\n";
            
            //Ninth line
            boardOutput << std::string(1, ' ');
            boardOutput << fifthHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << fifthHexagon->getSideColoredDisplay(2) << std::string(7, ' ')<< thirdHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << fourthHexagon->getSideColoredDisplay(3) << std::string(7, ' ') << ninthHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << ninthHexagon->getSideColoredDisplay(2) << "\n";

            //Tenth Line
            auto sixthHexagon = getHexagon(6);
            auto eighthHexagon = getHexagon(8);
            boardOutput<<fifthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<sixthHexagon->getCornerLabel(5)<<sixthHexagon->getSideColoredDisplay(0)<<sixthHexagon->getCornerLabel(1)<<std::string(13, ' ')<<eighthHexagon->getCornerLabel(0)<<eighthHexagon->getSideColoredDisplay(0)<<eighthHexagon->getCornerLabel(1)<<std::string(13, ' ')<<ninthHexagon->getCornerLabel(2)<<"\n";

            //Eleventh Line
            auto seventhHexagon = getHexagon(7);
            boardOutput<<std::string(1, ' ');
            boardOutput<<fifthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<sixthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<sixthHexagon->getSideColoredDisplay(1)<<std::string(9, ' ')<<eighthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<ninthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<ninthHexagon->getSideColoredDisplay(2)<<"\n";

            //Twelth Line
            boardOutput<<std::string(2, ' ');
            boardOutput<<fifthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<sixthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<thirdHexagon->getSideColoredDisplay(3)<<std::string(7, ' ')<<eighthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<ninthHexagon->getSideColoredDisplay(3)<<std::string(7, ' ')<<ninthHexagon->getSideColoredDisplay(1)<<"\n";

            //Thirteenth Line
            boardOutput<<std::string(3, ' ');
            boardOutput<<fifthHexagon->getCornerLabel(4)<<fifthHexagon->getSideColoredDisplay(1)<<sixthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<thirdHexagon->getCornerLabel(3)<<thirdHexagon->getSideColoredDisplay(1)<<eighthHexagon->getCornerLabel(2)<<std::string(13, ' ')<<ninthHexagon->getCornerLabel(4)<<ninthHexagon->getSideColoredDisplay(1)<<ninthHexagon->getCornerLabel(3)<<"\n";

            //Fourteenth Line
            auto tenthHexagon = getHexagon(10);
            auto fourteenthHexagon = getHexagon(14);
            boardOutput<<std::string(2, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<sixthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<seventhHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<eighthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<"\n";

            //Fifteenth Line
            boardOutput<<std::string(1, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<sixthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<seventhHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<eighthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<fourteenthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<fourteenthHexagon->getSideColoredDisplay(2)<<"\n";

            //Sixteenth Line
            boardOutput<<tenthHexagon->getCornerLabel(5)<<std::string(13, ' ') <<sixthHexagon->getCornerLabel(4)<<sixthHexagon->getSideColoredDisplay(1)<<sixthHexagon->getCornerLabel(3)<<std::string(13, ' ')<<seventhHexagon->getCornerLabel(2)<<eighthHexagon->getSideColoredDisplay(1)<<fourteenthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<fourteenthHexagon->getCornerLabel(2)<<"\n";

            //seventeenth Line
            auto eleventhHexagon = getHexagon(11);
            auto thirteenthHexagon= getHexagon(13);
            auto nineteenthHexagon = getHexagon(19);
            boardOutput<<std::string(1, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<eleventhHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<seventhHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<thirteenthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(2)<<"\n";

            //Eighteenth Line
            boardOutput<<std::string(2, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<eleventhHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<seventhHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<thirteenthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<fourteenthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<"\n";

            //Nineteenth Line
            boardOutput<<std::string(3, ' ');
            boardOutput<<tenthHexagon->getCornerLabel(4)<<tenthHexagon->getSideColoredDisplay(1)<<eleventhHexagon->getCornerLabel(5)<<std::string(13, ' ')<<seventhHexagon->getCornerLabel(3)<<seventhHexagon->getSideColoredDisplay(1)<<thirteenthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<fourteenthHexagon->getCornerLabel(4)<<fourteenthHexagon->getSideColoredDisplay(1)<<fourteenthHexagon->getCornerLabel(3)<<"\n";

            //Twentieth Line
            boardOutput<<std::string(2, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<eleventhHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<seventhHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<thirteenthHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<"\n";
        }

        std::cout << boardOutput.str();
    }
};

class BoardPrinter {
private:
    HexagonGrid grid;
    std::ostringstream boardOutput;

    void printBoardLine(const std::vector<std::shared_ptr<Hexagon>>& hexagonsInLine, int lineIndex) {
        std::cout << "  Printing board line " << lineIndex << " with " << hexagonsInLine.size() << " hexagons" << std::endl;
        std::string line;
        int currentSpace = 0;

        for (size_t i = 0; i < hexagonsInLine.size(); ++i) {
            auto hexagon = hexagonsInLine[i];
            if (!hexagon) {
                std::cout << "    Warning: Null hexagon at position " << i << std::endl;
                continue;
            }

            std::cout << "    Processing hexagon " << hexagon->getNumber() << " at position " << i << std::endl;

            auto connections = hexagon->getNeighborConnections();
            std::cout << "    Hexagon " << hexagon->getNumber() << " has " << connections.size() << " connections" << std::endl;

            // Calculate spacing
            int spacing = (i == 0) ? 10 : 14;
            if (i > 0) {
                auto prevHexagon = hexagonsInLine[i-1];
                auto [prevRow, prevCol] = prevHexagon->getGridPosition();
                auto [currRow, currCol] = hexagon->getGridPosition();
                if (currCol - prevCol == 1) {
                    spacing = 0; // Adjacent hexagons
                }
            }
            
            int spacesToAdd = std::max(0, spacing - (currentSpace % 14));
            std::cout << "    Adding " << spacesToAdd << " spaces" << std::endl;
            line += std::string(spacesToAdd, ' ');
            
            // Print line based on index
            switch (lineIndex) {
                case 0: // Top line
                    line += hexagon->getCornerLabel(0) + hexagon->getSideColoredDisplay(0) + hexagon->getCornerLabel(1);
                    break;
                case 1: // Upper slant
                    line += " " + hexagon->getSideColoredDisplay(5) + "         " + hexagon->getSideColoredDisplay(1);
                    break;
                case 2: // Upper middle slant
                    line += hexagon->getSideColoredDisplay(5) + "           " + hexagon->getSideColoredDisplay(1);
                    break;
                case 3: // Middle line
                    line += hexagon->getCornerLabel(5) + std::string(13, ' ');
                    if (connections.count(2)) {
                        auto [neighborNum, _, __] = connections[2];
                        auto neighborHexagon = grid.getHexagon(neighborNum);
                        if (neighborHexagon) {
                            line += neighborHexagon->getCornerLabel(5);
                        } else {
                            std::cout << "    Warning: Null neighbor hexagon " << neighborNum << std::endl;
                            line += hexagon->getCornerLabel(2);
                        }
                    } else {
                        line += hexagon->getCornerLabel(2);
                    }
                    break;
                case 4: // Lower middle slant
                    line += hexagon->getSideColoredDisplay(4) + "           " + hexagon->getSideColoredDisplay(2);
                    break;
                case 5: // Lower slant
                    line += " " + hexagon->getSideColoredDisplay(4) + "         " + hexagon->getSideColoredDisplay(2);
                    break;
                case 6: // Bottom line
                    line += "   " + hexagon->getCornerLabel(4) + hexagon->getSideColoredDisplay(3) + hexagon->getCornerLabel(3);
                    break;
            }
            std::cout << "    Current line length: " << line.length() << std::endl;
            currentSpace = line.length();
        }
        std::cout << "  Final line length: " << line.length() << std::endl;
        boardOutput << line << "\n";
    }

public:
    BoardPrinter(const HexagonGrid& grid) : grid(grid) {}
    void printBoard() {
        const auto& hexagons = grid.getHexagons();
        std::cout << "Number of hexagons: " << hexagons.size() << std::endl;
        // Add 25 spaces to the beginning of the boardOutput
        boardOutput << std::string(25, ' ');
        // Add the first hexagon's top line to the boardOutput
        if (!hexagons.empty()) {
            //First line
            auto firstHexagon = hexagons.begin()->second;

            boardOutput << firstHexagon->getCornerLabel(0)
                        << firstHexagon->getSideColoredDisplay(0)
                        << firstHexagon->getCornerLabel(1)
                        << "\n";
            //Second and Third line
            boardOutput << std::string(24, ' ');
            
            boardOutput << firstHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << firstHexagon->getSideColoredDisplay(1) << "\n";
        
            boardOutput << std::string(23, ' ');
        
            boardOutput << firstHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << firstHexagon->getSideColoredDisplay(2) << "\n";

            //Fourth line
            boardOutput << std::string(14, ' ');
            
            auto secondHexagon = grid.getHexagon(2);
            boardOutput << secondHexagon->getCornerLabel(0) << secondHexagon->getSideColoredDisplay(0)<< secondHexagon->getCornerLabel(1);
            auto fourthHexagon = grid.getHexagon(4);
            boardOutput << std::string(13, ' ') << fourthHexagon->getCornerLabel(0) <<fourthHexagon->getSideColoredDisplay(0)<< fourthHexagon->getCornerLabel(1)<< "\n";

            //Fifth line
            boardOutput << std::string(13, ' ');
            boardOutput << secondHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << secondHexagon->getSideColoredDisplay(1) ;
            boardOutput << std::string(9, ' ')<< fourthHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << fourthHexagon->getSideColoredDisplay(1) << "\n";

            //Sixth line
            boardOutput << std::string(12, ' ');
            boardOutput << secondHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << secondHexagon->getSideColoredDisplay(2) << std::string(7, ' ')<< fourthHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << fourthHexagon->getSideColoredDisplay(2) << "\n";

            //Seventh Line
            auto thirdHexagon = grid.getHexagon(3);
            auto fifthHexagon = grid.getHexagon(5);
            auto ninthHexagon = grid.getHexagon(9);

            boardOutput << std::string(3, ' ')<<fifthHexagon->getCornerLabel(0)<<fifthHexagon->getSideColoredDisplay(0);
            boardOutput << fifthHexagon->getCornerLabel(1) << std::string(13, ' ') << secondHexagon->getCornerLabel(2) 
            << thirdHexagon->getSideColoredDisplay(0)<< thirdHexagon->getCornerLabel(1) << std::string(13, ' ') << ninthHexagon->getCornerLabel(0) << ninthHexagon->getSideColoredDisplay(0) << ninthHexagon->getCornerLabel(1) << "\n";

            //Eighth line
            boardOutput << std::string(2, ' ');
            boardOutput << fifthHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << fifthHexagon->getSideColoredDisplay(1) << std::string(9, ' ') << thirdHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << fourthHexagon->getSideColoredDisplay(4) <<std::string(9, ' ')<< ninthHexagon->getSideColoredDisplay(5) << std::string(9, ' ') << ninthHexagon->getSideColoredDisplay(1) << std::string(9, ' ') <<  "\n";
            
            //Ninth line
            boardOutput << std::string(1, ' ');
            boardOutput << fifthHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << fifthHexagon->getSideColoredDisplay(2) << std::string(7, ' ')<< thirdHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << fourthHexagon->getSideColoredDisplay(3) << std::string(7, ' ') << ninthHexagon->getSideColoredDisplay(4) << std::string(11, ' ') << ninthHexagon->getSideColoredDisplay(2) << "\n";

            //Tenth Line
            auto sixthHexagon = grid.getHexagon(6);
            auto eighthHexagon = grid.getHexagon(8);
            boardOutput<<fifthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<sixthHexagon->getCornerLabel(5)<<sixthHexagon->getSideColoredDisplay(0)<<sixthHexagon->getCornerLabel(1)<<std::string(13, ' ')<<eighthHexagon->getCornerLabel(0)<<eighthHexagon->getSideColoredDisplay(0)<<eighthHexagon->getCornerLabel(1)<<std::string(13, ' ')<<ninthHexagon->getCornerLabel(2)<<"\n";

            //Eleventh Line
            auto seventhHexagon = grid.getHexagon(7);
            boardOutput<<std::string(1, ' ');
            boardOutput<<fifthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<sixthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<sixthHexagon->getSideColoredDisplay(1)<<std::string(9, ' ')<<eighthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<ninthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<ninthHexagon->getSideColoredDisplay(2)<<"\n";

            //Twelth Line
            boardOutput<<std::string(2, ' ');
            boardOutput<<fifthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<sixthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<thirdHexagon->getSideColoredDisplay(3)<<std::string(7, ' ')<<eighthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<ninthHexagon->getSideColoredDisplay(3)<<std::string(7, ' ')<<ninthHexagon->getSideColoredDisplay(1)<<"\n";

            //Thirteenth Line
            boardOutput<<std::string(3, ' ');
            boardOutput<<fifthHexagon->getCornerLabel(4)<<fifthHexagon->getSideColoredDisplay(1)<<sixthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<thirdHexagon->getCornerLabel(3)<<thirdHexagon->getSideColoredDisplay(1)<<eighthHexagon->getCornerLabel(2)<<std::string(13, ' ')<<ninthHexagon->getCornerLabel(4)<<ninthHexagon->getSideColoredDisplay(1)<<ninthHexagon->getCornerLabel(3)<<"\n";

            //Fourteenth Line
            auto tenthHexagon = grid.getHexagon(10);
            auto fourteenthHexagon = grid.getHexagon(14);
            boardOutput<<std::string(2, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<sixthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<seventhHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<eighthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<"\n";

            //Fifteenth Line
            boardOutput<<std::string(1, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<sixthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<seventhHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<eighthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<fourteenthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<fourteenthHexagon->getSideColoredDisplay(2)<<"\n";

            //Sixteenth Line
            boardOutput<<tenthHexagon->getCornerLabel(5)<<std::string(13, ' ') <<sixthHexagon->getCornerLabel(4)<<sixthHexagon->getSideColoredDisplay(1)<<sixthHexagon->getCornerLabel(3)<<std::string(13, ' ')<<seventhHexagon->getCornerLabel(2)<<eighthHexagon->getSideColoredDisplay(1)<<fourteenthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<fourteenthHexagon->getCornerLabel(2)<<"\n";

            //seventeenth Line
            auto eleventhHexagon = grid.getHexagon(11);
            auto thirteenthHexagon= grid.getHexagon(13);
            auto nineteenthHexagon = grid.getHexagon(19);
            boardOutput<<std::string(1, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<eleventhHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<seventhHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<thirteenthHexagon->getSideColoredDisplay(5)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(3)<<std::string(9, ' ')<<fourteenthHexagon->getSideColoredDisplay(2)<<"\n";

            //Eighteenth Line
            boardOutput<<std::string(2, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<eleventhHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<seventhHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<thirteenthHexagon->getSideColoredDisplay(4)<<std::string(11, ' ')<<fourteenthHexagon->getSideColoredDisplay(2)<<std::string(7, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<"\n";

            //Nineteenth Line
            boardOutput<<std::string(3, ' ');
            boardOutput<<tenthHexagon->getCornerLabel(4)<<tenthHexagon->getSideColoredDisplay(1)<<eleventhHexagon->getCornerLabel(5)<<std::string(13, ' ')<<seventhHexagon->getCornerLabel(3)<<seventhHexagon->getSideColoredDisplay(1)<<thirteenthHexagon->getCornerLabel(5)<<std::string(13, ' ')<<fourteenthHexagon->getCornerLabel(4)<<fourteenthHexagon->getSideColoredDisplay(1)<<fourteenthHexagon->getCornerLabel(3)<<"\n";

            //Twentieth Line
            boardOutput<<std::string(2, ' ');
            boardOutput<<tenthHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<eleventhHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<seventhHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<thirteenthHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<std::string(12, ' ')<<fourteenthHexagon->getSideColoredDisplay(1)<<"\n";
        }

        std::cout << boardOutput.str();
    }
};

void printHexagonSides(const std::shared_ptr<Hexagon>& hex, const std::string& name) {
    std::cout << name << " sides colors:" << std::endl;
    for (int i = 0; i < 6; ++i) {
        const auto& side = hex->getSides()[i];
        std::cout << "Side " << i << ": ";
        if (side.first) {
            std::cout << "First: " << side.first->getColor() << " (ID: " << side.first->getId() << ")";
        }
        if (side.second) {
            std::cout << ", Second: " << side.second->getColor() << " (ID: " << side.second->getId() << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
int main() {
    HexagonGrid grid;

    // Create 19 hexagons with different resource types and dice numbers
    grid.addHexagon('A', ResourceType::WOOD, 6);
    grid.addHexagon('B', ResourceType::BRICK, 8);
    grid.addHexagon('C', ResourceType::ORE, 9);
    grid.addHexagon('D', ResourceType::GRAIN, 5);
    grid.addHexagon('E', ResourceType::WOOL, 10);
    grid.addHexagon('F', ResourceType::WOOD, 3);
    grid.addHexagon('G', ResourceType::BRICK, 11);
    grid.addHexagon('H', ResourceType::ORE, 4);
    grid.addHexagon('I', ResourceType::GRAIN, 8);
    grid.addHexagon('J', ResourceType::WOOL, 12);
    grid.addHexagon('K', ResourceType::WOOD, 5);
    grid.addHexagon('L', ResourceType::BRICK, 6);
    grid.addHexagon('M', ResourceType::ORE, 3);
    grid.addHexagon('N', ResourceType::GRAIN, 11);
    grid.addHexagon('O', ResourceType::WOOL, 4);
    grid.addHexagon('P', ResourceType::WOOD, 9);
    grid.addHexagon('Q', ResourceType::BRICK, 10);
    grid.addHexagon('R', ResourceType::ORE, 2);
    grid.addHexagon('S', ResourceType::DESERT, 7);  // Desert hexagon

    // Attach hexagons to create the board layout
    // This is a simplified version and may need adjustment for a proper Catan board
    for (int i = 1; i <= 19; ++i) {
        auto hex = grid.getHexagon(i);
        if (!hex) {
            std::cerr << "Failed to create hexagon " << i << std::endl;
            return 1;
        }
        
        // Attach to neighbors (this is a simplified version and may need adjustment)
        if (i > 1) hex->attachHexagon(0, grid.getHexagon(i-1), 3);
        if (i < 19) hex->attachHexagon(3, grid.getHexagon(i+1), 0);
        if (i > 3) hex->attachHexagon(5, grid.getHexagon(i-3), 2);
        if (i < 17) hex->attachHexagon(2, grid.getHexagon(i+3), 5);
    }

    std::cout << "Initial board state:\n";
    grid.printBoard();

    // Change colors of some sides
    grid.getHexagon(1)->updateSideColor(5, "red");
    grid.getHexagon(7)->updateSideColor(2, "blue");
    grid.getHexagon(13)->updateSideColor(4, "green");
    grid.getHexagon(19)->updateSideColor(1, "yellow");

    std::cout << "\nBoard state after color changes:\n";
    grid.printBoard();

    std::cout << "Verifying all connections:" << std::endl;
    grid.verifyAllConnections();

    return 0;
}
