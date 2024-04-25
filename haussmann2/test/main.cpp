extern int f(int);
extern int g(int);

int main() { return f(f(g(-1))); }
