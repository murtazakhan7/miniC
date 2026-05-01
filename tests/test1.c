// test1.c — Lexer/Parser test: expressions, variables, basic I/O

int x = 10;
float pi = 3.14;
char grade = 'A';
bool flag = true;

int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(x, 5);
    float area = pi * 2.0;

    if (result > 10) {
        result = result - 1;
    } else {
        result = result + 1;
    }

    printf("%d", result);
    return 0;
}
