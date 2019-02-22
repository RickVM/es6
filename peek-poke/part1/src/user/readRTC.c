#include<stdio.h>

int main(int argc, char const *argv[])
{
    printf('Starting program');
    const char * p = (char *)0x40024000; 
    printf(p);
    printf("Finished.");
    return 0;
}
