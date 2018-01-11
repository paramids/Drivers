#ifndef ATLASI2C_H_
#define ATLASI2C_H_
#include <unistd.h>			//Needed for I2C port
#include <fcntl.h>			//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

extern int errno ;




typedef struct{
	int filerd_i2c; //file read
	int filewr_i2c; //file write
	uint32_t current_address;
	char res[32];
}AtlasI2C;


/*AlasI2C constructor & operations */
void AtlasI2C_ctor(AtlasI2C *const me, uint32_t address);
void AtlasI2C_set_i2c_addr(AtlasI2C *const me, uint32_t addr);
int AtlasI2C_write(AtlasI2C *const me, char* cmd);
char* AtlasI2C_read(AtlasI2C *const me, uint8_t num_bytes);
char* AtlasI2C_query(AtlasI2C *const me, char* str);
void AtlasI2C_close(AtlasI2C *const me);
void AtlasI2C_list_i2c_devices(AtlasI2C *const me, char* string);

char* s2upper(char * str);
bool startsWith(char *pre, char *str);

#endif /*ATLAS_H_*/


