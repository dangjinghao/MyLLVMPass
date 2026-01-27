// Test case for Loop Invariant Code Motion
// This function has a loop with invariant code that can be hoisted

int loop_invariant_test(int n, int a, int b) {
    int result = 0;
    
    // This loop has invariant computation: a * b
    // It should be hoisted outside the loop
    for (int i = 0; i < n; i++) {
        int invariant = a * b;  // a * b is loop invariant
        result += invariant;
    }
    
    return result;
}

int loop_with_multiple_invariants(int n, int x, int y, int z) {
    int sum = 0;
    
    // Multiple loop invariant computations
    for (int i = 0; i < n; i++) {
        int inv1 = x + y;      // loop invariant
        int inv2 = z * 2;      // loop invariant
        sum += inv1 + inv2;
    }
    
    return sum;
}

int main() {
    int result1 = loop_invariant_test(100, 5, 3);
    int result2 = loop_with_multiple_invariants(50, 10, 20, 7);
    return result1 + result2;
}
