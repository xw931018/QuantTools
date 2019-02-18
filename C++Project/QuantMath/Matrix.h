#include <iostream>
#include <vector>

using namespace std;

template <typename T>
class Matrix {
    private:
        unsigned rows_;
        unsigned cols_;
        vector<vector <T>> matrix_;

    public:
        Matrix();
        Matrix(unsigned rows, unsigned cols, const T& init);
        Matrix(const Matrix<T>& rhs);
        virtual ~Matrix() {}
        unsigned Rows() const { return rows_; }
        unsigned Cols() const { return cols_; }

        // Override () operator to get element; allow value change
        T& operator()(const unsigned& row, const unsigned& col);
        // Return const element whenn there is no value assignment
        const T& operator()(const unsigned& row, const unsigned& col) const;

        void PrintMatrix() const;

        Matrix<T>& operator=(const Matrix<T>& rhs); // Overriding assignment operator
        Matrix<T> operator+(const Matrix<T>& rhs); // Overriding addition operator with a matrix
        Matrix<T> operator+(const T& scalar);      // Overriding addition operator with a scalar value
        Matrix<T>& operator+=(const Matrix<T>& rhs); // Overriding cumulative addition operator with a matrix
        Matrix<T>& operator+=(const T& scalar); // Overriding cumulative addition operator with a matrix

        Matrix<T> operator-(const Matrix<T>& rhs); // Overriding subtraction operator with a matrix
        Matrix<T> operator-(const T& scalar);      // Overriding subtraction operator with a scalar value
        Matrix<T>& operator-=(const Matrix<T>& rhs); // Overriding cumulative subtraction operator with a matrix
        Matrix<T>& operator-=(const T& scalar); // Overriding cumulative subtraction operator with a matrix

        Matrix<T> operator*(const Matrix<T>& rhs);
        Matrix<T> operator*(const vector<T>& vec);
        Matrix<T> operator*(const T& scalar);
        Matrix<T>& operator*=(const Matrix<T>& rhs);
        Matrix<T>& operator*=(const vector<T>& vec);
        Matrix<T>& operator*=(const T& scalar);

        Matrix<T> operator/(const T& scalar);
        Matrix<T>& operator/=(const T& scalar);

        Matrix<T> transpose();

};