#ifndef JOINT_H_
#define JOINT_H_

#include <vector>
#include <list>
#include "coordinate.h"

/** constraints */
constexpr double MAX_HEIGHT = 10.5;
constexpr double MIN_HEIGHT = 9.5;
constexpr double MAX_WIDTH = 41;

/** each joint is indexed in the truss */
struct Joint : Coordinate
{
    Joint() : Coordinate{}, unknowns{ 0 }, neighbours{}, load{ 0 } {}
    Joint(double x, double y) : Coordinate{ x, y }, unknowns{ 0 }, neighbours{}, load{ 0 } {}

    bool operator==(const Joint & other) const {
        return other.x == x && other.y == y;
    }

    virtual bool posIsValid() {
        return x > 0 && x < MAX_WIDTH
            && y > 0 && y < MAX_HEIGHT;
    }

    void shiftPos(double dx, double dy) {
        x += dx;
        y += dy;
    }

    int unknowns;
    std::vector<int> neighbours; /*indices of neighbouring joints*/
    double load;
};

/**
 * A joint in the truss that has the high ground.
*/
struct HighestJoint : Joint {

    HighestJoint() : Joint{} {}
    HighestJoint(double x, double y) : Joint{ x, y } {}

    bool posIsValid() override {
        std::cout << "CHECKED!" << std::endl;
        return x > 0 && x < MAX_WIDTH
            && y > MIN_HEIGHT && y < MAX_HEIGHT;
    }
};

#endif