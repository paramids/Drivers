
#include <unistd.h>			//Needed for I2C port
#include <fcntl.h>			//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include "pca9685.h"

//Utility imports
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

int errnum;//For Error Handling

/*Static variables for I2C communications*/
uint32_t long_timeout = 2;
uint16_t short_timeout = 1;


uint32_t default_address = 0x40;		//Default slave address

/* Atlas I2C Constructor*/
void Pca9685_ctor(Pca9685 *const me, uint32_t address){
	
	default_address = address;
	
	
	/*Open two file systems, one for reading & one
	for writing*/
	
	//----- OPEN THE I2C BUS -----
	if ((me->filerd_pca9685 = open(file_read, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		/*printf("Failed to open the read i2c bus");
		errnum = errno;
  		fprintf(stderr, "Value of errno: %d\n", errno);
	        perror("Error printed by perror");
     		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));*/
		return;
	}
	if ((me->filewr_pca9685 = open(file_write, O_RDWR)) < 0)
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
	Pca9685_set_i2c_addr(me, address);
	Pca9685_init(me, address);
}

/*Set I2C Address method*/
void Pca9685_set_i2c_addr(Pca9685 *const me, uint32_t addr){
	/*Set the I2C to the slave specified by the address
	The commands for I2C dev using the ioctl functions
	are specified in the i2c-dev.h file from i2c-tools*/

	// #DEFINE I2C_SLAVE = 0x703;

	//printf("set I2C Address Method...\n");

	if (ioctl(me->filerd_pca9685, I2C_SLAVE, addr) < 0)
	{
		//printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}
	if (ioctl(me->filewr_pca9685, 0x703, addr) < 0)
	{
		//printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}

	me->current_address = addr;
}

/*Initialize the pca9685 module*/
void Pca9685_init(Pca9685 *const me, uint32_t addr){
	/*Write mode 1 address
	Respond to ALLCALL address, goto low power mode, Register Autoincrement Enabled*/
	Pca9685_write(me,MODE1,0b00110001);
	//delay 50us
	usleep(500);
	// PWM frequency PRE_SCALE address
	Pca9685_write(me,0xfe,0x04);
	//delay 50us
	usleep(500);

	/*Write mode 1 address*/
	// Set to our prefered mode[ Reset, INT_CLK, Auto-Increment, Normal Mode] 
	Pca9685_write(me,MODE1,0xa1);
	//delay 50us
	usleep(500);
	
	/*Write mode 2 address
	Set to our prefered mode[Output logic state not inverted, Outputs change on STOP, 
	totem pole structure, When OE = 1 (output drivers not enabled), LEDn = 0]*/
	Pca9685_write(me,MODE1,0b00000100);
	//delay 50us
	usleep(500);
		
}

/*Write to I2C slave device*/
int Pca9685_write(Pca9685 *const me, uint8_t reg, uint8_t val){
	uint8_t buff[2]={0};
	buff[0]=reg;
	buff[1]=val;
	
	if (write(me->filewr_pca9685, buff, 2) != 2)		
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
}	

/*Read from I2C slave device*/
char* Pca9685_read(Pca9685 *const me,uint8_t num_bytes){
	char buffer[31];
	memset(buffer, '\0', sizeof(buffer));
	const char s[2] = "\0";
   	char *token;
   	
	memset(me->res, '\0', sizeof(me->res));
	
	//----- READ BYTES -----
 							
	if (read(me->filerd_pca9685, buffer, num_bytes) != num_bytes){
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


void pca9685_close(Pca9685 *const me){
	close(1);
	if (close(me->filerd_pca9685)< 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		//printf("Failed to close the i2c bus\n");
		
	}
	if (close(me->filewr_pca9685) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		//printf("Failed to close the i2c bus\n");
		
	}
}

char* Pca9685_set_pwm(Pca9685 *const me, uint8_t channel, uint8_t value){
		
	Pca9685_write(me,LED0 + 4 * channel, value);
	return NULL;
}

char* Pca9685_set_all_pwm(Pca9685 *const me, uint8_t channel, uint8_t value){
		
	Pca9685_write(me,0xfc, value);
	return NULL;
}

int main(void){
	
}









