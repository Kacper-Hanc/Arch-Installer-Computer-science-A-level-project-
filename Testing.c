#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void array_to_string(char **array,char *str,int lim){
    char *p = str;
    for (int i=0;i<lim;i++){
        size_t len = strlen(array[i]);
        if (len > 0 && array[i][len - 1] == '\n') {
            len--;
        }
        memcpy(p, array[i], len);
        p += len;

        if (i < lim - 1) {
            *p++ = '-';
        }
    }
    *p = '\0';
}
int main(void)
{
    char *arr[3] = {
        "apple",
        "banana",
        "cherry"
    };
    char str[50];
    array_to_string(arr,str,3);
    printf(str);
   
    return 0;
}
