#ifndef PCA9685_H_
#define PCA9685_H_
#include <unistd.h>			//Needed for I2C port
#include <fcntl.h>			//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>


/*Registers/etc:*/
#define PCA9685_ADDRESS 0x40
#define MODE1 0x00
#define MODE2 0x01
#define SUBADR1 0x02
#define SUBADR2 0x03
#define SUBADR3 0x04
#define PRESCALE 0xFE
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09
#define ALL_LED_ON_L 0xFA
#define ALL_LED_ON_H 0xFB
#define ALL_LED_OFF_L 0xFC
#define ALL_LED_OFF_H 0xFD

/* Bits:*/
#define RESTART 0x80
#define SLEEP 0x10
#define ALLCALL 0x01
#define INVRT 0x10
#define OUTDRV 0x04

const char *file_read = (char*)"/dev/i2c-1"; //default bus
const char *file_write = (char*)"/dev/i2c-1"	;//default bus

typedef struct{
	int filerd_pca9685; //file read
	int filewr_pca9685; //file write
	uint32_t current_address;
	char res[32];
}Pca9685;

/*AlasI2C constructor & operations */
void Pca9685_ctor(Pca9685 *const me, uint32_t address);
void Pca9685_config(Pca9685 *const me, uint32_t addr);
int Pca9685_write(Pca9685 *const me, char* reg, int cmd);
char* Pca9685_read(Pca9685 *const me, uint8_t num_bytes);
int Pca9685_set_pwm_freq(Pca9685 *const me, char* cmd);
char* Pca9685_set_pwm(Pca9685 *const me, uint8_t num_bytes);
char* Pca9685_set_all_pwm(Pca9685 *const me, char* str);
void Pca9685_close(Pca9685 *const me);
void Pca9685_software_reset(Pca9685 *const me, char* string);

#endif /*ATLAS_H_*/


