#include <iostream>
#include "truss.h"
#include "matrix.h"
using namespace std;

int main() {
    Truss t{};
    int jNum, mNum, loadNum;
    cin >> jNum >> mNum >> loadNum;

    // add joints
    for (int i = 0; i < jNum; ++i) {
        int x, y;
        char free, high;
        cin >> x >> y >> free >> high;
        t.addJoint(x, y, free == 'T' || free == 't', high == 'T' || high == 't');
    }

    // link joints (add members)
    for (int i = 0; i < mNum; ++i) {
        int j1, j2;
        cin >> j1 >> j2;
        t.addMember(j1, j2);
    }

    // add loads to joints
    for (int i = 0; i < loadNum; ++i) {
        int jointNum;
        double load;
        cin >> jointNum >> load;
        t.addLoad(jointNum, load);
    }

    t.setDisplacements(4, 0.5);
    double baseCost;
    constexpr double TOL = 5E-7;
    do {
        baseCost = t.getOptimalCost();
        t.optimize();
        t.showBest();
    } while (t.getOptimalCost() + TOL < baseCost);

    cout << "done!" << endl;

    char exit;
    cin >> exit;
    return 0;
}