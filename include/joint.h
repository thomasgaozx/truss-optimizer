#ifndef JOINT_H_
#define JOINT_H_

#include <memory>
#include <vector>
#include <list>
#include "coordinate.h"

/* constraints for joint positions */
class BaseConstraint {
public:
    BaseConstraint() = default;
    virtual ~BaseConstraint() = default;

    /* returns true if the given `j` is in constraint range */
    virtual bool checkPos(const Coordinate & j) {
        return j.x > 0 && j.x < MAX_WIDTH && j.y > 0 && j.y < MAX_HEIGHT;
    }

protected:
    static constexpr double MAX_HEIGHT = 10.5;
    static constexpr double MIN_HEIGHT = 9.5;
    static constexpr double MAX_WIDTH = 41;
};

/** each joint is indexed in the truss */
struct Joint : Coordinate
{
    Joint() : Coordinate{}, unknowns{ 0 }, neighbours{}, load{ 0 },
        constraint{ std::make_unique<BaseConstraint>() } { }

    Joint(double x, double y)
        : Coordinate{ x, y }, unknowns{ 0 }, neighbours{}, load{ 0 },
        constraint{ std::make_unique<BaseConstraint>() } { }

    Joint(double x, double y, std::unique_ptr<BaseConstraint> && customConstraints)
        : Coordinate{ x, y }, unknowns{ 0 }, neighbours{}, load{ 0 },
        constraint{ std::move(customConstraints) } { }

    bool operator==(const Joint & other) const {
        return other.x == x && other.y == y;
    }

    bool posIsValid() {
        return constraint->checkPos(*this);
    }

    void shiftPos(double dx, double dy) {
        x += dx;
        y += dy;
    }

    int unknowns;
    std::vector<int> neighbours; /*indices of neighbouring joints*/
    double load;

    std::unique_ptr<BaseConstraint> constraint;
};

#endif