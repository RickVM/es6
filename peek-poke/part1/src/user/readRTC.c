#include<stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    printf("Starting program\n");
    const char * p = (char *)0x40024000; 
    printf("Value in memory: %d \n", *p);
    printf("Finished.\n");
    return 0;
}
