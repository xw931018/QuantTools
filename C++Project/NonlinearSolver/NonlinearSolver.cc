#include <iostream>
#include <cmath>
#include "./NonlinearSolver.h"

using namespace std;

NonlinearSolver::NonlinearSolver(double (*function)(double x)) {
    myFunc = function;
}


// DON'T USE THE SAME NAME AS MEMBER ATTRIBUTES!!!!!!!
SteffensenSolver::SteffensenSolver(double (*function)(double x), double initial) : x0(initial),
 NonlinearSolver::NonlinearSolver( function ){
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

void SteffensenSolver::setParams(double x0, double tol, long maxIter) {
    x0 = x0;
    tol = tol;
    maxIter = maxIter;
}

double SteffensenSolver::solve(double x0, double tol, long maxIter) {
    setParams(x0, tol, maxIter);
    double sol = solve();
    return sol;
}



double F(double x) {
    return (x - 5) * (x - 1);
}

int main() {
    double x0 = 10;

    SteffensenSolver solver(F, x0);
    double solution = solver.solve();
    cout << "Solution is \n:" << solution << ' ' << F(solution)<< endl;
}

