#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256

int main(){
   int fd;
   printf("Starting peek and poke test program.\n");
   fd = open("/dev/pwm", O_RDWR);             // Open the device with read/write access
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
      
      if (scanf("%d%c", &num, &term) != 2 || term != " \n") {
         printf("\n");

         char reg[100];
         char value[100];
         int len = 0;

         switch (num)
         {
            case 1:

               // Read register
               printf("Enter register address: ");
               scanf("%s", reg);
               
               // Value for register
               printf("Enter value to write: ");
               scanf("%s", value);

               printf("Register %s and value %s\n", reg, value);
               
               // Packaging the adress and value to write into a message buffer
               char messsage[BUFFER_LENGTH];
               sprintf(messsage, "%s|%s", reg, value);

               int ret = write(fd, messsage, strlen(messsage));
               if (ret < 0) {
                  perror("Failed to update the register on the device to the new value.\n");
                  return errno;
               }

               printf("Succesfully writen value %s to register adress %s\n", value, reg);
               
               break;

            case 2:
               // read from kernel given an adress
               printf("Enter register address to read from: ");
               scanf("%s", reg);
               
               printf("And length: ");
               scanf("%d", &len);

               printf("Reading value of adress %s with length %d... !\n", reg, len);

               char message[BUFFER_LENGTH];
               sprintf(message, "%s|%d", reg, len);

               ret = read(fd, message, BUFFER_LENGTH);

               if ( ret < 0 ) {
                  perror("Failed to read the value of the register adress!\n");
                  return errno;
               }

               printf("Succesfully read register with adress %s, value is %s\n", reg, message);
               
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