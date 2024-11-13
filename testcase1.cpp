#include <iostream>
using namespace std;



int main() {

    // all uint32_t to represent unsigned integers (registers) that are 32 bits long
    uint32_t x1, x2, x3, x4, x5, x6, x7, x8, x9, x10; 
    x1 = 12;
    x2 = 5678;

    x3 = x1 + x2; // ADD x3,x1,x2
    x4 = x1 - x2; // SUB x4,x1,x2

    // SLT x5,x1,x2
     if (x1 < x2){

            x5 = 1;
        }
        else{

            x5 = 0;
        }
    x6 = x1 | x2; // OR x6,x1,x2
    x7 = x1 << 1;  // SLLI x7,x1,1 (double x1)
    x8 = x4 ^ x6; // XOR x8,x4,x6
    x9 = x1 >> 3;  // SRAI x9,x1,3 (divide x1 by 8)
    x10 = x2 >> 4; // SRLI x10,x2,4 (divide x2 by 16)

    return 0;
}
