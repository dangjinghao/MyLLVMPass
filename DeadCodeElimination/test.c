// Test cases for Dead Code Elimination Pass

// Test 1: Simple unused computation
int test_simple_dead(int a, int b) {
    int dead1 = a + b;        // Dead: result not used
    int dead2 = a * 2;        // Dead: result not used
    return a;
}

// Test 2: Chain of dead code (should be removed recursively)
int test_chain_dead(int a, int b) {
    int x = a + 1;            // Dead
    int y = x * 2;            // Dead (depends on x)
    int z = y + 3;            // Dead (depends on y)
    return b;
}

// Test 3: Partially dead code (only unused parts should be removed)
int test_partial_dead(int a, int b) {
    int used = a + b;         // Used: kept
    int dead = a * 2;         // Dead: removed
    return used;
}

// Test 4: Multiple uses (should NOT be removed)
int test_multiple_uses(int a) {
    int x = a + 1;            // Used twice, keep it
    int y = x * 2;            // Used
    int z = x + 3;            // Used
    return y + z;
}

// Test 5: Side effects (should NOT be removed)
int global_var = 0;

int test_side_effects(int a) {
    int dead = a + 1;         // Dead but may have side effects in optimization
    global_var = a;           // Has side effect, keep it
    return a * 2;
}

// Test 6: Complex chain with branching
int test_complex(int a, int b, int cond) {
    int dead1 = a * 3;        // Dead
    int used1 = a + b;        // Used
    
    if (cond) {
        int dead2 = b * 2;    // Dead in this block
        return used1;
    } else {
        int dead3 = a - b;    // Dead in this block
        return used1 + 1;
    }
}

// Test 7: No dead code
int test_no_dead(int a, int b) {
    int x = a + b;
    int y = x * 2;
    return y;
}

// Test 8: All dead except return
int test_all_dead(int a) {
    int x = a + 1;            // Dead
    int y = a * 2;            // Dead
    int z = a - 3;            // Dead
    return 42;                // Only this survives
}
