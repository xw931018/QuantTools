#include <vector>
#include <iostream>
#include "Matrix.h"

using namespace std;

template<typename T>
Matrix<T>::Matrix() {
    rows_ = 0;
    cols_ = 0;
}

template<typename T>
Matrix<T>::Matrix(unsigned rows, unsigned cols, const T& init) : rows_(rows), cols_(cols) {
    matrix_.resize(rows_);
    for (unsigned i = 0; i < rows_; i++) {
        matrix_[i].resize(cols_, init);
    }
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& rhs) {
    matrix_ = rhs.matrix_;
    rows_   = rhs.Rows();
    cols_   = rhs.Cols();
}

template<typename T>
T& Matrix<T>::operator()(const unsigned& row, const unsigned& col) {
    return matrix_[row][col];
}

template<typename T>
const T& Matrix<T>::operator()(const unsigned& row, const unsigned& col) const {
    return matrix_[row][col];
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs) {
    if (this == &rhs) {
        return *this;
    }
    rows_ = rhs.Rows();
    cols_ = rhs.Cols();
    matrix_.resize(rows_);
    for (unsigned i = 0; i < rows_; i++) {
        matrix_[i].resize(cols_);
        for (unsigned j = 0; j < cols_; j++) {
            matrix_[i][j] = rhs(i, j);
        }
    }
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& rhs) {
    Matrix<T> result(rows_, cols_, 0.0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            result(i, j) = matrix_[i][j] + rhs(i, j);
        }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(const T& scalar) {
    Matrix<T> result(rows_, cols_, 0.0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            result(i, j) = matrix_[i][j] + scalar;
        }
    return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& rhs) {
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            matrix_[i][j] += rhs(i, j);
        }
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const T& scalar) {
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            matrix_[i][j] += scalar;
        }
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& rhs) {
    Matrix<T> result(rows_, cols_, 0.0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            result(i, j) = matrix_[i][j] - rhs(i, j);
        }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator-(const T& scalar) {
    Matrix<T> result(rows_, cols_, 0.0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            result(i, j) = matrix_[i][j] - scalar;
        }
    return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& rhs) {
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            matrix_[i][j] -= rhs(i, j);
        }
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(const T& scalar) {
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            matrix_[i][j] -= scalar;
        }
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& rhs) {
    unsigned new_rows = rows_;
    unsigned new_cols = rhs.Cols();
    Matrix<T> result(new_rows, new_cols, 0.0);
    for (unsigned i = 0; i < new_rows; i++)
        for (unsigned j = 0; j < new_cols; j++)
            for (unsigned k = 0; k < cols_; k++) {
                result(i, j) += matrix_[i][k] * rhs(k, j);
            }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const vector<T>& vec) {
    Matrix<T> result(rows_, 1, 0.0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned k = 0; k < cols_; k++) {
            result(i, 0) += matrix_[i][k] * vec[k];
        }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const T& scalar) {
    Matrix<T> result(rows_, cols_, 0.0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            result(i, j) = matrix_[i][j] * scalar;
        }
    return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& rhs) {
    Matrix<T> result = (*this) * rhs;
    *this = result;
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const vector<T>& vec) {
    Matrix<T> result = (*this) * vec;
    *this = result;
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const T& scalar) {
    Matrix<T> result = (*this) * scalar;
    *this = result;
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator/(const T& scalar) {
    Matrix<T> result(rows_, cols_, 0);
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            result(i, j) = matrix_[i][j] / scalar;
        }
    return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator/=(const T& scalar) {
    for (unsigned i = 0; i < rows_; i++)
        for (unsigned j = 0; j < cols_; j++) {
            matrix_[i][j] /= scalar;
        }
    return *this;
}

int main() {
    //Test matrix constructor;
    Matrix<double> mat(10, 3, 5.5);
    cout << mat.Rows() << ' ' << mat.Cols() << ' ' << mat(1, 2) << endl;
    mat(1, 2) = 3;
    cout << mat(1, 2) << ' ' << mat(2, 2) << endl;
    // Test matrix copy constructor
    Matrix<double> mat1 = mat;
    Matrix<double> mat2;
    mat2 = mat;
    cout << mat1.Rows() << ' ' << mat1.Cols() << ' ' << mat1(1, 2) << endl;
    cout << mat2.Rows() << ' ' << mat2.Cols() << ' ' << mat2(1, 2) << endl;
    cout << (mat1 + mat2)(1, 2) << endl;
    cout << (mat1 + 200)(1, 2) << endl;
    cout << (mat1 += 200)(1, 2) << endl;
    cout << (mat1 -= 200)(1, 2) << endl;

    // Testing matrix multiplication
    Matrix<double> mat3(10, 3, 5.2);
    Matrix<double> mat4(3, 7, 3.333);
    Matrix<double> mat5 = mat3 * mat4;
    cout << mat5.Rows() << ' ' << mat5.Cols() << ' ' << mat5(2, 5) << endl;
}
