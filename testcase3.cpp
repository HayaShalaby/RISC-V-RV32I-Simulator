#include <iostream>
using namespace std;

int main() {
    uint32_t x1 = 10 << 12; // LUI x1, 10 (Load upper immediate to x1 (x1 = 10 << 12))
    uint32_t x2 = 5 << 12; // LUI x2, 5 (Load upper immediate to x2 (x2 = 5 << 12))
    uint32_t x3 = 0, x4 = 0, x5 = 0;  // Initialize x3, x4, x5

    // all uint32_t to represent unsigned integers (registers) that are 32 bits long

    for (int i = 0; i < 3; i++) {
        // BLTU branch: if x1 is less than x2, exit the loop
        if (x1 < x2) {
            break;
        } else {
            x3 = x1 | x2;
        }

        x4 = x3 << 1; // SLLI x4, x3, 1(Shift x3 left by 1 and place value in x4)

        // SLTU x5, x4, x1 (Set x5 to 1 if x4 < x1 (unsigned comparison), otherwise set to 0)
        if (x4 < x1){
            x5 = 1;
        }
        else{
            x5 = 0;
        }
    }

    return 0;
}
