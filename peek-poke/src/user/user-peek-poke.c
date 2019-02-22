#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>

int main(){
   int fd;
   printf("Starting device test code example...\n");
   fd = open("/dev/peek-poke", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   } else {
      printf("Succesfully opened peek-poke device driver!");
   }
   return 0;
}