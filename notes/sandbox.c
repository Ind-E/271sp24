#include <stdio.h>


int main()
{
    char *ptr0 = "", *ptr1 = "";
    char str0[5] = "zero", str1[5] = "one_" ; // why 5?
    printf("p: ptr0: %p, ptr1: %p, str0: %p, str1: %p\n", ptr0, ptr1, str0, str1) ;
    printf("s: ptr0: %s, ptr1: %s, str0: %s, str1: %s\n", ptr0, ptr1, str0, str1) ;
    return 0 ;
}