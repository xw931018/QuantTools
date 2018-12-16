#include <iostream>
#include <cmath>
#include <functional>
#include "./NonlinearSolver.h"

using namespace std;

NonlinearSolver::NonlinearSolver(std::function<double(double)> f) {
    myFunc = f;
}


// DON'T USE THE SAME NAME AS MEMBER ATTRIBUTES!!!!!!!
SteffensenSolver::SteffensenSolver(std::function<double(double)> f, double initial) : x0(initial),
 NonlinearSolver::NonlinearSolver( f ){
    tol = 1e-7;
    maxIter = 100;
}

double SteffensenSolver::solve() {
    double temp, step;
    xPrevious = x0;
    for (int i = 0; i < maxIter; i++) {
        temp = myFunc(xPrevious);
        step = (myFunc(xPrevious + temp) - temp) / temp;
        step = temp / step;
        // cout << i << ' ' << step << endl;
        xCurrent = xPrevious - step;
        xPrevious = xCurrent;
        if (abs(step) < tol) {
            return xCurrent;
        }
    }
    return xCurrent;
}

void SteffensenSolver::setParams(double initial, double tol, long maxIter) {
    x0 = initial;
    tol = tol;
    maxIter = maxIter;
}

double SteffensenSolver::solve(double initial, double tol, long maxIter) {
    setParams(initial, tol, maxIter);
    double sol = solve();
    return sol;
}



double F(double x) {
    return (x - 5) * (x - 1);
}

//int main() {
//    double x0 = 10;
//
//    SteffensenSolver solver(F, x0);
//    double solution = solver.solve(3, 1e-2, 100000);
//    cout << "Solution is \n:" << solution << ' ' << F(solution)<< endl;
//}

