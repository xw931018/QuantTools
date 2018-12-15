
class NonlinearSolver {
    public:
        double (*myFunc) (double);
        double tol;

        NonlinearSolver(double (*function)(double));
        virtual double solve() = 0;
        virtual double solve(double, double, long) = 0;
};

class SteffensenSolver : public NonlinearSolver {
    private:
        double x0, xPrevious, xCurrent;
        long maxIter;
        void   setParams(double, double, long);

    public:
        SteffensenSolver(double (*function)(double), double);
        double solve();
        double solve(double, double, long);

};