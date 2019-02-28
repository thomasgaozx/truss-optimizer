#include <algorithm>
#include "matrix.h"
using namespace std;

int MatrixSolver::rref(Mat & mat) {
    int i = 0, j = 0;
    int headRow = 0;
    int rank = 0;

    for (; i < rows && j < cols; ++j, headRow = i) {
        for (; i < rows && abs(mat[i][j]) < TOL; ++i) {}

        if (i != rows) {
            scale(mat, i, 1.0 / mat[i][j]);
            if (i != headRow)
                swap(mat, i, headRow);

            for (i += 1; i < rows; ++i)
                if (abs(mat[i][j]) > TOL)
                    addTo(mat, i, headRow, -mat[i][j]);

            for (i = headRow - 1; i > -1; --i)
                if (abs(mat[i][j]) > TOL)
                    addTo(mat, i, headRow, -mat[i][j]);

            ++rank;
            i = headRow + 1;
        } else
            i = headRow;
    }

    return rank;
}

int MatrixSolver::getLeading1Pos(const Mat & mat, int row) {
    int pos = 0;
    int colSize = mat[0].size();
    
    // ensure all entries before leading 1 is 0
    for (; mat[row][pos] == 0; ++pos)
        if (pos == colSize - 2)
            return -1;
    
    int l1 = pos++;

    // ensure all entries after leading 1 is 0 (till the last entry)
    for (; pos < colSize - 1 && mat[row][pos] == 0; ++pos) { }

    return pos == colSize - 1 ? l1 : -1;
}

void MatrixSolver::swap(Mat & mat, int row1, int row2) {
    std::swap(mat[row1], mat[row2]);
}

void MatrixSolver::scale(Mat & mat, int row, double magnitude) {
    for (int j = 0; j < cols; ++j) {
        mat[row][j] *= magnitude;
        if (abs(mat[row][j]) < TOL)
            mat[row][j] = 0;
    }
}

void MatrixSolver::addTo(Mat & mat, int row1, int row2, double multipleOfRow2) {
    for (int j = 0; j < cols; ++j) {
        mat[row1][j] += multipleOfRow2 * mat[row2][j];
        if (abs(mat[row1][j]) < TOL)
            mat[row1][j] = 0;
    }
}
