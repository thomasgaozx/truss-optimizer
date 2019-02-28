#include <limits>
#include <queue>
#include <iostream>
#include "truss.h"
#include "matrix.h"
using namespace std;

constexpr double UNSOLVED = numeric_limits<double>::max();
const double PI = atan(1.0) * 4;

using Direction = Coordinate; /* reuse exact same member fields */

Truss::Truss() : joints{}, freeJoints{}, internalForces{}, displacements{},
        bestTruss{ UNSOLVED, vector<Coordinate>() } {}

/**
 * Specialized constraint class for a joint that has the high ground
 * (with additional position constraints)
*/
class HighestJointConstraint : public BaseConstraint {
public:
    bool checkPos(const Coordinate & j) override {
        return j.x > 0 && j.x < MAX_WIDTH
            && j.y > MIN_HEIGHT && j.y < MAX_HEIGHT;
    }
};

void Truss::addJoint(double x, double y, bool free, bool highest) {
    joints.push_back(highest ?
        Joint{ x, y, make_unique<HighestJointConstraint>() } : Joint{ x, y });
    bestTruss.second.push_back(Coordinate{ x, y });

    if (free)
        freeJoints.push_back(joints.size() - 1);
}

bool Truss::addMember(int i, int j) {
    if (i >= (int)joints.size() || j >= (int)joints.size())
        return false;
    joints[i].neighbours.push_back(j);
    joints[j].neighbours.push_back(i);

    joints[i].unknowns += 1;
    joints[j].unknowns += 1;

    internalForces[Member{ i, j }] = UNSOLVED;
    return true;
}

void Truss::addLoad(int jointNum, double load) {
    joints[jointNum].load += load;
}

double Truss::getOptimalCost() const {
    return bestTruss.first;
}

bool Truss::setDisplacements(int num, double radius) {
    if (num < 3 || radius <= 0)
        return false;

    double deltaTheta = 2 * PI / num;
    double theta = 0;

    displacements.push_back(Displacement{}); // default

    for (int i = 0; i < num; ++i, theta += deltaTheta)
        displacements.push_back(
            Displacement{ cos(theta) * radius, sin(theta) * radius });
    return true;
}

Direction unitVecFrom(const Coordinate & c1, const Coordinate & c2) {
    double dx = c2.x - c1.x;
    double dy = c2.y - c1.y;
    double len = sqrt(pow(dx, 2) + pow(dy, 2));

    return Direction{ dx / len, dy / len };
}

bool Truss::solveJoint(int jointNum) {
    if (joints[jointNum].unknowns == 0)
        return false; // already solved, no new progress made

    vector<Member> unknownForces;
    Direction knownForce{ 0, joints[jointNum].load };
    bool progress{ false };

    // set up system of equations
    Mat mat = Mat(2, vector<double>(joints[jointNum].unknowns + 1));
    int unknownPos = 0;
    for (int neighbour : joints[jointNum].neighbours) {
        Member edge{ jointNum, neighbour };
        Direction u = unitVecFrom(joints[jointNum], joints[neighbour]);

        double internalForce = internalForces[edge];
        if (internalForce == UNSOLVED) {
            unknownForces.push_back(edge);
            mat[0][unknownPos] = u.x;
            mat[1][unknownPos] = u.y;
            ++unknownPos;
        } else {
            knownForce.x += u.x * internalForce;
            knownForce.y += u.y * internalForce;
        }
    }
    mat[0][unknownPos] = -knownForce.x;
    mat[1][unknownPos] = -knownForce.y;

    // solve system of equations
    MatrixSolver solver{ mat };
    int rank = solver.rref(mat);

    // process solved unknowns
    for (int row = 0; row < rank; ++row) {
        int solvedPos = solver.getLeading1Pos(mat, row);

        if (solvedPos > -1) { // solved unknown
            const auto & member = unknownForces[solvedPos];
            internalForces[member] = mat[row][unknownForces.size()];
            --joints[member.j1].unknowns;
            --joints[member.j2].unknowns;
            progress = true;
        }
    }
    return progress;
}

void Truss::resetInternalForces() {
    for (auto & kvpair : internalForces) {
        if (kvpair.second != UNSOLVED) {
            kvpair.second = UNSOLVED;
            joints[kvpair.first.j1].unknowns += 1;
            joints[kvpair.first.j2].unknowns += 1;
        }
    }
}

void Truss::optimizeImpl(int freeJointNum) {
    if (freeJointNum == freeJoints.size()) {
        solve();
        evaluateCost();
        return;
    }

    auto & target = joints[freeJoints[freeJointNum]];

    for (auto delta : displacements) {
        target.shiftPos(delta.x, delta.y);

        if (target.posIsValid())
            optimizeImpl(freeJointNum + 1);

        target.shiftPos(-delta.x, -delta.y);
    }
}

bool Truss::solve() {
    resetInternalForces();
    auto comp = [this](int j1, int j2) /*min heap  --> greater comparator*/
            { return joints[j1].unknowns > joints[j2].unknowns; };

    /* best effort min heap: solve equations with minimum unknowns first.
       It is only best-effort because the unknowns of a random joint in
       the heap will be updated whenever a joint is solved. */
    priority_queue<int, vector<int>, decltype(comp)> pq{ comp };
    deque<int> dump;

    int joint = 0;
    for (; joint < (int)joints.size(); ++joint)
        pq.push(joint);

    bool progress = true;
    while (progress && pq.size() != 0) {
        progress = false;

        while (pq.size() != 0) {
            joint = pq.top();
            pq.pop();
            if (solveJoint(joint))
                progress = true;

            if (joints[joint].unknowns != 0)
                dump.push_back(joint);
        }

        // re-prioritizing
        while (dump.size() != 0) {
            pq.push(dump.front());
            dump.pop_front();
        }
    }

    return pq.size() == 0;
}

double getLen(Coordinate c1, Coordinate c2) {
    return sqrt(pow(c2.x - c1.x, 2) + pow(c2.y - c1.y, 2));
}

void Truss::evaluateCost() {
    double cost = 0;
    for (const auto & kvpair : internalForces) {
        const auto & member = kvpair.first;
        cost += getLen(joints[member.j1], joints[member.j2]) * abs(kvpair.second);
    }

    // compare and swap
    if (cost < bestTruss.first) {
        bestTruss.first = cost;

        for (int i = 0; i < (int)joints.size(); ++i)
            bestTruss.second[i] = joints[i]; // copy on assignment
    }
}

void Truss::optimize() {
    optimizeImpl(0);

    // reset joints to optimal coordinate
    int pos = 0;
    for (const auto & joint : bestTruss.second) {
        joints[pos].x = joint.x;
        joints[pos].y = joint.y;
        ++pos;
    }
}

void Truss::display() const {
    // print joints
    for (int i = 0; i < (int)joints.size(); ++i)
        cout << (char)('A' + i) << ": (" << joints[i].x << ", "
            << joints[i].y << ")" << endl;

    // print internal forces
    for (const auto & kvpair : internalForces)
        cout << "Member " << (char)('A' + kvpair.first.j1)
            << (char)('A' + kvpair.first.j2) << ": " << kvpair.second <<endl;
}

void Truss::showBest() const {
    cout << "Cost: " << bestTruss.first << endl;

    int pos = 0;
    for (const auto & c : bestTruss.second)
        cout << (char)('A' + pos++) << ": (" << c.x << ", " << c.y << ")" << endl;
}
