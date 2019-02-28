#ifndef MATRIX_H_
#define MATRIX_H_

#include <cmath>
#include <vector>

using Mat = std::vector<std::vector<double>>;

class MatrixSolver {
public:
    MatrixSolver(const Mat & mat)
        : rows{ (int)mat.size() }, cols{ (int)mat[0].size() } {}

    /** reduce the matrix to rref, returns the rank of the matrix */
    int rref(Mat & mat);

    /** if the row is solved, return leading 1's pos , otherwise return -1 */
    int getLeading1Pos(const Mat & mat, int row);

private:
    void swap(Mat & mat, int row1, int row2);
    void scale(Mat & mat, int row, double magnitude);
    void addTo(Mat & mat, int row1, int row2, double multipleOfRow2);

    int rows;
    int cols;

    static constexpr double TOL = 1E-11;
};

#endif
