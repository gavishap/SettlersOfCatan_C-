#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <string>

class Side {
public:
    virtual std::string display() const = 0;
    virtual ~Side() = default;
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
    char label;
public:
    Corner(char label) : label(label) {}
    char getLabel() const { return label; }
    void setLabel(char newLabel) { label = newLabel; }
};

class Hexagon : public std::enable_shared_from_this<Hexagon> {
    std::vector<std::shared_ptr<Side>> sides;
    std::map<int, std::shared_ptr<Corner>> corners;
    std::map<int, std::weak_ptr<Hexagon>> neighbors;

public:
    Hexagon(char startCorner = 'A') {
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

    void display() const {
        std::cout << "   " << corners.at(0)->getLabel() << sides[0]->display() << corners.at(1)->getLabel() << std::endl;
        std::cout << "  " << sides[5]->display() << "         " << sides[1]->display() << std::endl;
        std::cout << " " << sides[5]->display() << "           " << sides[1]->display() << std::endl;
        std::cout << corners.at(5)->getLabel() << "             " << corners.at(2)->getLabel() << std::endl;
        std::cout << " " << sides[4]->display() << "         " << sides[2]->display() << std::endl;
        std::cout << "  " << sides[4]->display() << "       " << sides[2]->display() << std::endl;
        std::cout << "   " << corners.at(4)->getLabel() << sides[3]->display() << corners.at(3)->getLabel() << std::endl;
    }

    void attachHexagon(int thisSide, std::shared_ptr<Hexagon> otherHexagon, int otherSide) {
        neighbors[thisSide] = otherHexagon;
        otherHexagon->neighbors[otherSide] = shared_from_this();

        // Adjust corners
        int thisCorner1 = thisSide;
        int thisCorner2 = (thisSide + 1) % 6;
        int otherCorner1 = otherSide;
        int otherCorner2 = (otherSide + 1) % 6;

        corners[thisCorner1] = otherHexagon->corners[otherCorner2];
        corners[thisCorner2] = otherHexagon->corners[otherCorner1];
    }
};

class HexagonBoard {
    std::vector<std::shared_ptr<Hexagon>> hexagons;

public:
    void addHexagon(char startCorner = 'A') {
        hexagons.push_back(std::make_shared<Hexagon>(startCorner));
    }

    void attachHexagons(int hexIndex1, int side1, int hexIndex2, int side2) {
        if (hexIndex1 < hexagons.size() && hexIndex2 < hexagons.size()) {
            hexagons[hexIndex1]->attachHexagon(side1, hexagons[hexIndex2], side2);
        }
    }

    void displayBoard() const {
        for (const auto& hexagon : hexagons) {
            hexagon->display();
            std::cout << std::endl;
        }
    }
};

int main() {
    HexagonBoard board;

    board.addHexagon('A');
    board.addHexagon('G');

    // Attach the second hexagon "under left" of the first
    board.attachHexagons(0, 5, 1, 2);

    board.displayBoard();

    return 0;
}