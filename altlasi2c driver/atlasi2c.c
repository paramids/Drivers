
#include <unistd.h>			//Needed for I2C port
#include <fcntl.h>			//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include "atlasi2c.h"

//Utility imports
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

int errnum;//For Error Handling

/*Static variables for I2C communications*/
uint32_t long_timeout = 2;
uint16_t short_timeout = 1;

uint32_t default_address = 0x5a;		//Default slave address

/* Atlas I2C Constructor*/
void AtlasI2C_ctor(AtlasI2C *const me, uint32_t address){
	
	default_address = address;
	
	
	/*Open two file systems, one for reading & one
	for writing*/
	
	//----- OPEN THE I2C BUS -----
	if ((me->filerd_i2c = open(file_read, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		/*printf("Failed to open the read i2c bus");
		errnum = errno;
  		fprintf(stderr, "Value of errno: %d\n", errno);
	        perror("Error printed by perror");
     		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));*/
		return;
	}
	if ((me->filewr_i2c = open(file_write, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		/*printf("Failed to open the i2c bus");
		errnum = errno;
  		fprintf(stderr, "Value of errno: %d\n", errno);
	        perror("Error printed by perror");
     		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));*/
		return;
	}

	/*initializes I2C to either a user specified or default address*/
	
	AtlasI2C_set_i2c_addr(me, address);
}

/*Set I2C Address method*/
void AtlasI2C_set_i2c_addr(AtlasI2C *const me, uint32_t addr){
	/*Set the I2C to the slave specified by the address
	The commands for I2C dev using the ioctl functions
	are specified in the i2c-dev.h file from i2c-tools*/

	// #DEFINE I2C_SLAVE = 0x703;

	//printf("set I2C Address Method...\n");

	if (ioctl(me->filerd_i2c, 0x703, addr) < 0)
	{
		//printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}
	if (ioctl(me->filewr_i2c, 0x703, addr) < 0)
	{
		//printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}

	me->current_address = addr;
}

/*Write to I2C slave device*/
int AtlasI2C_write(AtlasI2C *const me, char* cmd){

	//printf("I2C Write Method...\n");
	
	char *zero = "00";
	char cmd_str[10];
	strcpy(cmd_str, cmd);
	strcat(cmd_str, zero);
	//strcat(cmd, "00");
	
	//printf("After concat...\n");
	int length = 10;//strlen(cmd_str);
	/*write() returns the number of bytes actually written,
	if it doesn't match then an error occurred (e.g. no response from the device)*/

	if (write(me->filewr_i2c, cmd_str, length) != length)		
	{
		/* ERROR HANDLING: i2c transaction failed */
		/*printf("Failed to write to the i2c bus in write method.\n");
		errnum = errno;
  		fprintf(stderr, "Value of errno: %d\n", errno);
	        perror("Error printed by perror");
     		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));*/
		return 0;
	}else{
		return 1;	
	}
	//printf("End of write");

}	

/*Read from I2C slave device*/
char* AtlasI2C_read(AtlasI2C *const me,uint8_t num_bytes){
	char buffer[31];
	memset(buffer, '\0', sizeof(buffer));
	const char s[2] = "\0";
   	char *token;
   	
	memset(me->res, '\0', sizeof(me->res));
	
	//----- READ BYTES -----
	if (read(me->filerd_i2c, buffer, num_bytes) != num_bytes)//read() returns the number of bytes actually read, if it doesn't match then an error 								occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		/*printf("Failed to read from the i2c bus.\n");
		errnum = errno;
  		fprintf(stderr, "Value of errno: %d\n", errno);
	        perror("Error printed by perror");
     		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));*/
		return NULL;
	}
	else
	{
		//printf("Data read: %s\n", buffer);
		/*Start processing the response here*/
		/* get the first token */
   		token = strtok(buffer, s);
   		/* walk through other tokens */
		while( token != NULL ) {
	      		printf( " %s\n", token );
      			strcat(me->res, token); //append token to 
      			token = strtok(NULL, s);
      		}
		if(me->res[0]==84){
 		/*Convert MSB (first 4 bits) to 0- 
		This is due to a raspberry pi hw glitch*/
			for(int i=1; i<=sizeof(me->res);i++){
				me->res[i]= me->res[i] & ~(8<<4); 
			}
		return me->res;
		}
		else{
			return NULL;
		}
			 
   	}
		
}


	
char* AtlasI2C_query(AtlasI2C *const me, char* str){
	/*write command to current addressed device using I2C Command*/
	AtlasI2C_write(me, str);

	/*Decode response*/
	//printf( "After Write\n");

	char string[10];
	strcpy(string, str);


	if(startsWith(s2upper(string),"R")||startsWith(s2upper(string),"CAL")){
		//printf( "Print long-time\n");
		sleep(long_timeout);
	}else if(startsWith(s2upper(string),"SLEEP")){
		//printf( "Sleepmode\n");	
	}else{
		//printf( "Print Short time\n");
		sleep(long_timeout);	
	}
	return AtlasI2C_read(me,31);

}


void AtlasI2C_close(AtlasI2C *const me){
	if ((me->filerd_i2c = close(me->filerd_i2c)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		//printf("Failed to close the i2c bus\n");
		
	}
	if ((me->filewr_i2c = close(me->filewr_i2c)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		//printf("Failed to close the i2c bus\n");
		
	}
}

void AtlasI2C_list_i2c_devices(AtlasI2C *const me, char* string){
/*TODO*/
}

//Starts with method
bool startsWith(char *pre, char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

//String to upper method
char* s2upper(char * str){
   int i = 0;
   while(str[i]) {
      str[i]=(toupper(str[i]));
      i++;
   }
   return str;

}







