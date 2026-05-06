// test_opt.c — Combined optimization test: CF + DCE + CSE

int main() {
    int x = 1;
    int y = 2;

    // Constant Folding (CF)
    int folded = 8 * 4;      // -> 32
    bool cmp = 10 > 2;       // -> true

    // Common Subexpression Elimination (CSE)
    int a = x + y;
    int b = x + y;           // same expression as above

    // Dead Code Elimination (DCE)
    int dead1 = 123;
    int dead2 = dead1 + 7;   // never used

    printf("%d", a + b + folded);
    return 0;
}
