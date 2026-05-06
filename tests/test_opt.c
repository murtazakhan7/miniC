// test_opt.c — Test constant folding and optimizations

int main() {
    // Constant folding opportunity
    int a = 10 + 5;      // Should fold to 15
    int b = 3 * 4;       // Should fold to 12
    int c = 100 / 10;    // Should fold to 10
    
    // Comparisons that can fold
    bool t1 = 5 < 10;    // Should fold to true
    bool t2 = 15 > 20;   // Should fold to false
    
    // Dead code (unused temporaries)
    int dead = 99;       // unused
    int x = a + b;
    
    printf("%d", x);
    return 0;
}
