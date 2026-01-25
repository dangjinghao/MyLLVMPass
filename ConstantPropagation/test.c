#include <stdbool.h>

int test_arithmetic(int x, int y) {
    // Constant folding: both operands are constants
    int a = 3 + 5;           // => 8
    int b = 10 - 4;          // => 6
    int c = 6 * 7;           // => 42
    int d = 20 / 4;          // => 5
    
    // Identity operations
    int e = x + 0;           // => x
    int f = 0 + x;           // => x
    int g = x - 0;           // => x
    int h = x * 1;           // => x
    int i = 1 * x;           // => x
    int j = x / 1;           // => x
    
    // Zero operations
    int k = x * 0;           // => 0
    int l = 0 * x;           // => 0
    
    // Self operations
    int m = x - x;           // => 0
    
    return a + b + c + d + e + f + g + h + i + j + k + l + m;
}

int test_bitwise(int x, int y) {
    // Bitwise AND
    int a = x & 0;           // => 0
    int b = 0 & x;           // => 0
    int c = x & -1;          // => x (all bits set)
    int d = -1 & x;          // => x
    int e = x & x;           // => x (idempotent)
    
    // Bitwise OR
    int f = x | 0;           // => x
    int g = 0 | x;           // => x
    int h = x | -1;          // => -1
    int i = -1 | x;          // => -1
    int j = x | x;           // => x (idempotent)
    
    // Bitwise XOR
    int k = x ^ 0;           // => x
    int l = 0 ^ x;           // => x
    int m = x ^ x;           // => 0 (self-XOR)
    
    // Shift operations
    int n = x << 0;          // => x
    int o = x >> 0;          // => x
    int p = 0 << x;          // => 0
    int q = 0 >> x;          // => 0
    
    return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q;
}

int test_comparison(int x, int y) {
    // Self comparisons
    bool a = (x == x);       // => true
    bool b = (x != x);       // => false
    bool c = (x < x);        // => false
    bool d = (x > x);        // => false
    bool e = (x <= x);       // => true
    bool f = (x >= x);       // => true
    
    return a + b + c + d + e + f;
}

float test_float(float x, float y) {
    // Float constant folding
    float a = 2.5f + 3.5f;   // => 6.0
    float b = 10.0f - 4.5f;  // => 5.5
    float c = 2.0f * 3.0f;   // => 6.0
    float d = 8.0f / 2.0f;   // => 4.0
    
    // Float identity operations
    float e = x + 0.0f;      // => x
    float f = 0.0f + x;      // => x
    float g = x - 0.0f;      // => x
    float h = x * 1.0f;      // => x
    float i = 1.0f * x;      // => x
    float j = x / 1.0f;      // => x
    
    // Float zero operations
    float k = x * 0.0f;      // => 0.0
    float l = 0.0f * x;      // => 0.0
    
    // Float self operations
    float m = x - x;         // => 0.0
    
    // Float comparisons
    bool n = (x == x);       // => true
    bool o = (x != x);       // => false (assuming no NaN)
    bool p = (x < x);        // => false
    bool q = (x <= x);       // => true
    
    return a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q;
}

int test_select(int x, int y, bool cond) {
    // Select with constant condition
    int a = cond ? 10 : 20;
    int b = true ? x : y;    // => x
    int c = false ? x : y;   // => y
    
    // Select with same values
    int d = cond ? x : x;    // => x
    
    return a + b + c + d;
}

int test_complex(int x, int y) {
    // Chained operations that should be optimized
    int a = ((x + 0) * 1) - 0;           // => x
    int b = (x | 0) & -1;                // => x
    int c = ((x ^ 0) << 0) >> 0;         // => x
    int d = (x & x) | (x & 0);           // => x
    int e = (x * 0) + (y * 1);           // => y
    
    // Operations that create constants
    int f = (x - x) + (y ^ y);           // => 0
    
    bool g = (x == x) && (y <= y);       // => true
    bool h = (x != x) || (y < y);        // => false
    
    return a + b + c + d + e + f + g + h;
}

unsigned int test_unsigned(unsigned int x) {
    // Unsigned operations
    unsigned int a = x / 1u;             // => x
    bool b = (x <= x);                   // => true (unsigned)
    bool c = (x < x);                    // => false (unsigned)
    
    return a + b + c;
}

int main() {
    int x = 42, y = 17;
    float fx = 3.14f, fy = 2.71f;
    bool cond = true;
    
    int result = 0;
    result += test_arithmetic(x, y);
    result += test_bitwise(x, y);
    result += test_comparison(x, y);
    result += (int)test_float(fx, fy);
    result += test_select(x, y, cond);
    result += test_complex(x, y);
    result += (int)test_unsigned((unsigned int)x);
    
    return result;
}