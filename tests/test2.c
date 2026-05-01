// test2.c — Tests: loops, arrays, structs, nested control flow

struct Point {
    int x;
    int y;
};

int arr[5] = {1, 2, 3, 4, 5};

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int sum_array(int arr[], int len) {
    int total = 0;
    int i = 0;
    while (i < len) {
        total = total + arr[i];
        i = i + 1;
    }
    return total;
}

int main() {
    // for loop with break/continue
    int i;
    for (i = 0; i < 10; i++) {
        if (i == 5) break;
        if (i == 3) continue;
        printf("%d", i);
    }

    // do-while
    int count = 0;
    do {
        count = count + 1;
    } while (count < 5);

    // ternary
    int x = 10;
    int abs_x = x < 0 ? -x : x;

    // compound assignment
    x += 5;
    x -= 2;
    x *= 3;

    // bitwise
    int flags = 0xFF & 0x0F;
    flags = flags | 0x10;
    flags = flags ^ 0x01;
    flags = flags << 2;
    flags = flags >> 1;

    // logical operators
    bool a = true;
    bool b = false;
    bool c = a && !b;
    bool d = a || b;

    int fact = factorial(6);
    printf("%d", fact);

    return 0;
}
