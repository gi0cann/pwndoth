#include <stdio.h>

int main() {
    char name[20];
    puts("Hello from vuln");
    scanf("%s", name);
    printf("Hello %s from vuln\n", name);
}
