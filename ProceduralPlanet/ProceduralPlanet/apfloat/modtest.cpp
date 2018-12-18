#include <iostream>
#include <sstream>
#include "ap.h"


using namespace std;


// Program to test the modint class

int main (int argc, char *argv[])
{
    int m;
    modint a, b;

    if (argc < 2)
    {
        cerr << "USAGE: modtest m" << endl;
        return 2;
    }

    istringstream s (argv[1]);

    if (!(s >> m))
    {
        cerr << "Invalid argument m: " << argv[1] << endl;
        return 1;
    }

    setmodulus (moduli[m]);

    a = 600000000000;
    b = 600000000000;

    cout << "sizeof (modint) = " << sizeof (modint) << endl;

    cout << a << " * " << modint (600000000000) << " = " << a * modint (600000000000) << " (mod " << modint::modulus << ")" << endl;

    cout << a << " ^ " << b << " = " << pow (a, b) << " (mod " << modint::modulus << ")" << endl;

    cout << 1 << " / " << a << " = " << (b = modint (1) / a) << " (mod " << modint::modulus << ")" << endl;

    cout << a << " * " << b << " = " << a * b << " (mod " << modint::modulus << ")" << endl;

    cout << "Primitive root of " << modint::modulus << " = " << primitiveroots[m] << endl;

    clearmodulus ();

    return 0;
}
