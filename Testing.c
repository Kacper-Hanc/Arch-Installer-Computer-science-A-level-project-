#include <stdarg.h>
#include <stdio.h>

#define MAKE_MSG(buf, fmt, ...) \
    snprintf(buf, sizeof(buf), fmt, __VA_ARGS__)


void func1(char *var1, char *var2) {
    printf("%s\n",var1);
    printf("%s",var2);
}
int main(void) {
    char var1[50] = "Title";
    char message[50];
    MAKE_MSG(message, "this is the title %s", var1);
    MAKE_MSG(var1,"[ %s ]",var1);
    func1(var1, message);
    printf("%s\n", var1);
    return 0;
}
