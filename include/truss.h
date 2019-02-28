#ifndef TRUSS_H_
#define TRUSS_H_

#include <list>
#include <unordered_map>
#include "member.h"

class Truss
{
public:
    Truss();

    void addJoint(double x, double y, bool free = true, bool highest = false);
    bool addMember(int i, int j);
    void addLoad(int jointNum, double load);

    double getOptimalCost() const;

    /** Sets the possible displacements of a free joints. */
    bool setDisplacements(int num, double radius);
    
    /**
     * Solve the truss and in the meantime checks constraint (optional)
     * Return false if there is statistical indeterminacy or other errors.
    */
    bool solve();

    /**
     * Evaluates the truss configuration with provided cost model, if it is more 
     * cost-effective than the recorded one, update the recorded one
    */
    void evaluateCost();

    /** Optimize the truss recursively (1 cycle) */
    void optimize();

    /** Print out the joints and members forces respectively */
    void display() const;

    /** Print out the cost and joint coordinates of the best truss */
    void showBest() const;

private:
    std::vector<Joint> joints;
    std::vector<int> freeJoints;
    std::unordered_map<Member, double> internalForces;

    using Displacement = Coordinate;
    std::list<Displacement> displacements;

    std::pair<double, std::vector<Coordinate>> bestTruss; /* cost:joint_coordinates */

    /** set up system of equations and attempt solving the joint*/
    bool solveJoint(int jointNum);

    /** resets all internal forces to unknown */
    void resetInternalForces();

    /** recursive optimization w/ backtracking*/
    void optimizeImpl(int freeJoint);
};

#endif

