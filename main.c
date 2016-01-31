#include <iostream>
#include <errno.h>
#include <pigpio.h>
#include <stdint.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "build.h"

using namespace std;

int fd_rgb;

char RGB_result[8];
char MMA_result[6];
char RGB_check[1];

float color[4];//actual color reading
float accel[3];//actual accelerometer reading


char MMA_init_buf[] = {4, MMA8451_ADDR,  // set up Chip address
                       2, 7, 2, MMA8451_CTRL_REG1, 0x00, 3,    // put MMA in standby
                       2, 7, 2, MMA8451_XYZ_DATA_CFG, 0x01, 3, // Configure XYZ
		       2, 7, 2, MMA8451_CTRL_REG1, 0x21,3,	//initialize MMA
                       0 // EOL
                       };

char MMA_read_buf[] = {4, MMA8451_ADDR,  // set up Chip address
                       2, 7, 1, MMA8451_OUT_X_MSB, 2, 6, 6, 3,  //read 6 bytes following MMA8451_OUT_X_MSB
                       0 // EOL
                       };

char RGB_init_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ATIME), 0xEB, 3,    // 
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_CONFIG), 0x00, 3, // 
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_CONTROL), 0x01, 3, // 
		       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ENABLE), TCS34725_ENABLE_PON,3,	//power on
                       0 // EOL
                       };

char RGB_enable_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ENABLE), (TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN), 3,    // 
                       0 // EOL
                       };

char RGB_check_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 1, (RGB_COMMAND_BIT | TCS34725_ID), 3,
		       2, 6, 1, 3, 
                       0 // EOL
                       };

int main(){

if (gpioInitialise() < 0) return 1;

// Open bit banging I2C on standard I2C pins , SDA-2, SCL-3
if (bbI2COpen(2, 3, 100000)) return 1;

//init MMA
(void)bbI2CZip(2, MMA_init_buf,sizeof(MMA_init_buf),NULL,0);

//init RGB
(void)bbI2CZip(2, RGB_init_buf,sizeof(RGB_init_buf),NULL,0);
//wait
for (int i = 0; i<10; i++){};
//enable RGB
(void)bbI2CZip(2, RGB_enable_buf,sizeof(RGB_enable_buf),NULL,0);

//check
int rcnt = bbI2CZip(2, RGB_check_buf,sizeof(RGB_check_buf),RGB_check,1);
printf("ID = %c", RGB_check[0]);
printf("number of bytes received = %d \n",rcnt);




for(int counter =0; counter < 500; counter ++){
	/***
	//Clear, Red, BLue, Green
	fd_rgb = wiringPiI2CSetup(RGB_ADDR);
	RGB_result[0] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |CLEAR_L));
	RGB_result[1] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |CLEAR_H));
	RGB_result[2] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |RED_L));
	RGB_result[3] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |RED_H));
	RGB_result[4] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |BLUE_L));
	RGB_result[5] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |BLUE_H));
	RGB_result[6] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |GREEN_L));
	RGB_result[7] = wiringPiI2CReadReg8(fd_rgb,(RGB_COMMAND_BIT |GREEN_H));
	
	//Color_RGB
	color[0]= ((RGB_result[1]<<8) + RGB_result[0]);
	color[1]= ((RGB_result[3]<<8) + RGB_result[2]);
	color[2]= ((RGB_result[5]<<8) + RGB_result[4]);
	color[3]= ((RGB_result[7]<<8) + RGB_result[6]);


	***/

	//X_MSB,X_LSB,Y_MSB,Y_LSB,Z_MSB,Z_LSB
	(void)bbI2CZip(2, MMA_read_buf,sizeof(MMA_read_buf),MMA_result,sizeof(MMA_result));

	


	//Accel XYZ
	accel[0]= ((MMA_result[0]<<8) + MMA_result[1]);
	accel[1]= ((MMA_result[2]<<8) + MMA_result[3]);
	accel[2]= ((MMA_result[4]<<8) + MMA_result[5]);

	//printf("CLEAR = %.2f  " ,color[0]);
	//printf("RED = %.2f  "   ,color[1]);
	//printf("BLUE = %.2f  "  ,color[2]);
	//printf("GREEN = %.2f  " ,color[3]);
	printf("MMA_result = %c  "   ,MMA_result[0]);
	printf("MMA_result = %c  "   ,MMA_result[1]);
	printf("MMA_result = %c  "   ,MMA_result[2]);
	printf("MMA_result = %c  "   ,MMA_result[3]);
	printf("MMA_result = %c  "   ,MMA_result[4]);
	printf("MMA_result = %c  "   ,MMA_result[5]);

	printf("X = %.2f  "   ,accel[0]);
	printf("Y = %.2f  "   ,accel[1]);
	printf("Z = %.2f  \n" ,accel[2]);
	
}
  bbI2CClose(2);

  gpioTerminate();
}