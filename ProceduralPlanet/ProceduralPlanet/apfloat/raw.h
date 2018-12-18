#if !defined(__RAW_H)
#define __RAW_H


#include <iostream>
#include <iomanip>


// The modint raw data type, unsigned if possible
typedef unsigned __int64 rawtype;


const rawtype MAXMODULUS = 0xFFFFFFFFFFFFFFFF;
const rawtype MAXRAW = 0xFFFFFFFFFFFFFFFF;
const int RAWBITS = 64;

// Base (must be BASEDIGIT^BASEDIGITS = BASE < all MODULI)
const rawtype BASE = 10000000000000000000;
const rawtype BASEDIGIT = 10;
const int BASEDIGITS = 19;
const rawtype BASEFACTORS[] = {2, 5};           // All prime factors of BASE
const int NBASEFACTORS = 2;                     // Number of prime factors of BASE

const rawtype MODULI[3] = {18446744069414584321, 18446744056529682433, 18446742974197923841};     // upto n = 3*2^32
const rawtype PRIMITIVEROOTS[3] = {7, 10, 19};
const rawtype WFTAMODULI[3] = {0, 0, 0};                // not used
const rawtype WFTAPRIMITIVEROOTS[3] = {0, 0, 0};

const size_t MAXTRANSFORMLENGTH = (size_t) 6442450944; // Maximum transform length for these moduli


// Low-level inline functions

// Visual C++

extern "C"
{
rawtype modmul (rawtype a, rawtype b, rawtype m);
rawtype modadd (rawtype a, rawtype b, rawtype m);
rawtype modsub (rawtype a, rawtype b, rawtype m);
}

inline std::ostream &operator<< (std::ostream &str, rawtype r)
{
    unsigned p1, p2, p3;

    p1 = r / 1000000000000000000;
    r %= 1000000000000000000;
    p2 = r / 1000000000;
    p3 = r % 1000000000;

    if (p1) str << p1 << std::setw (9) << std::setfill ('0');
    if (p1 || p2) str << p2 << std::setw (9) << std::setfill ('0');
    str << p3;

    return str;
}


#endif  // __RAW_H
