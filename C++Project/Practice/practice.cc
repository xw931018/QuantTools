#include <iostream>

using namespace std;

class B {
    private:
        double* b;
    public:
        B() {
            b = new double (1.0);
        }
        virtual ~B() {
            cout << "Destructor of B " << endl;
            delete b;
        }
};

class D : public B {
    private:
        int* d;
    public:
        D(int N) {
            d = new int[N];
        }
        ~D() {
            cout << "Destructor of D " << endl;
            delete[] d;
        }
};

int main() {
    {
        B* d = new D(10);
        delete d;
    }


}