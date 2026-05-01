// test3.c — Tests: structs, member access, type casting, sizeof, hex/octal literals

struct Student {
    int id;
    float gpa;
    char grade;
};

void print_student(struct Student s) {
    printf("%d", s.id);
    printf("%d", s.grade);
}

int main() {
    struct Student stu;
    stu.id    = 42;
    stu.gpa   = 3.75;
    stu.grade = 'A';

    print_student(stu);

    // type cast
    float f = 9.99;
    int   n = (int)f;

    // hex and octal literals
    int hex = 0xFF;
    int oct = 0777;
    int combined = hex & oct;

    // pre/post increment and decrement
    int k = 5;
    int m = k++;
    int p = ++k;
    k--;
    --k;

    // nested ternary
    int score = 75;
    int grade_val = score >= 90 ? 4 :
                    score >= 80 ? 3 :
                    score >= 70 ? 2 : 1;

    printf("%d", grade_val);
    return 0;
}
