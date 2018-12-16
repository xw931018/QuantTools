#include <functional>

class NonlinearSolver {
    public:
        std::function<double(double)> myFunc;
        double tol;

        NonlinearSolver(std::function<double(double)> f);
        virtual double solve() = 0;
        virtual double solve(double, double, long) = 0;
};

class SteffensenSolver : public NonlinearSolver {
    private:
        double x0, xPrevious, xCurrent;
        long maxIter;
        void   setParams(double, double, long);

    public:
        SteffensenSolver(std::function<double(double)> f, double);
        double solve();
        double solve(double, double, long);

};