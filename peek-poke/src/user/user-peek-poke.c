#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>

int main(){
   int fd;
   printf("Starting device test code example...\n");
   fd = open("/dev/peek-poke", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...\n");
      return errno;
   } else {
      printf("Succesfully opened peek-poke device driver!\n");
   }

   while (1) {
      int num;
      char term;
      printf("\nPlease choose an option ...\n");
      printf("\t[1] Press one for writing data to a register\n");
      printf("\t[2] Press two for reading x amount of data from registers\n");
      printf("\t[9] Press nine for exiting this program\n\n");
      if (scanf("%d%c", &num, &term) != 2 || term != ' \n') {
         switch (num)
         {
            case 1:
               printf("Thank you for selecting option one.\n");
               break;
            case 2:
               printf("You sir, are awesome!\n");
               break;
            case 9: 
               exit(0);
               break;
            default:
               printf("Not a valid number, try again!\n");
            break;
         }
      } else {
         printf("Invalid input, try again!\n");
      }
   }

   return 0;
}