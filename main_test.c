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

int RGB_handler;
int MMA_handler;

char RGB_result[8];
char MMA_result[6];
char RGB_check[1];

int err; //error code

float color[4];//actual color reading
float accel[3];//actual accelerometer reading


char RGB_init_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ATIME), 0xEB, 3,    // 
                      // 2, 7, 2, (RGB_COMMAND_BIT | TCS34725_CONFIG), 0x00, 3, // 
                      // 2, 7, 2, (RGB_COMMAND_BIT | TCS34725_CONTROL), 0x01, 3, // 
		      // 2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ENABLE), TCS34725_ENABLE_PON,3,	//power on
                       0 // EOL
                       };

char RGB_enable_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ENABLE), (TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN), 3,    // 
                       0 // EOL
                       };

char RGB_check_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 1, (RGB_COMMAND_BIT | TCS34725_ID),3,
		       2, 6, 1, 3, 
                       0 // EOL
                       };

int main(){

if (gpioInitialise() < 0) return -1;

if (bbI2COpen(2,3,100000) != 0) return -1;

err = bbI2CZip(2,RGB_init_buf,sizeof(RGB_init_buf),NULL,0);

printf ("RGB init error = %d\n", err);

for (int i=0; i<1000;i++){};

err = bbI2CZip(2,RGB_enable_buf,sizeof(RGB_enable_buf),NULL,0);

printf ("RGB enable error = %d\n", err);

err = bbI2CZip(2,RGB_check_buf,sizeof(RGB_check_buf),RGB_check,sizeof(RGB_check) );

/***
RGB_handler = i2cOpen(1,RGB_ADDR,0);

i2cWriteByteData(RGB_handler, (RGB_COMMAND_BIT | TCS34725_ATIME),0xEB);
i2cWriteByteData(RGB_handler,(RGB_COMMAND_BIT | TCS34725_CONFIG), 0x00); 
i2cWriteByteData(RGB_handler,(RGB_COMMAND_BIT | TCS34725_CONTROL), 0x01);

i2cWriteByteData(RGB_handler,(RGB_COMMAND_BIT | TCS34725_ENABLE), TCS34725_ENABLE_PON);
for(int i= 0; i<5;i++);
i2cWriteByteData(RGB_handler,(RGB_COMMAND_BIT | TCS34725_ENABLE), (TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));

int RGB_ID_check = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT | TCS34725_ID));

***/
printf("RGB check ID error = %d\n", err);
if(RGB_check[0] == 0x44){
	printf("RGB Sensor Connected, ID = %d\n", RGB_check[0]);
}else{
	printf("ERROR! RGB Sensor not found\n");
	bbI2CClose(2);
	gpioTerminate();
	return -1;
}
for(int counter =0; counter < 100; counter ++){
	//Clear, Red, BLue, Green
	RGB_result[0] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |CLEAR_L));
	RGB_result[1] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |CLEAR_H));
	RGB_result[2] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |RED_L));
	RGB_result[3] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |RED_H));
	RGB_result[4] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |BLUE_L));
	RGB_result[5] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |BLUE_H));
	RGB_result[6] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |GREEN_L));
	RGB_result[7] = i2cReadByteData(RGB_handler,(RGB_COMMAND_BIT |GREEN_H));
	
	
	//Color_RGB
	color[0]= ((RGB_result[1]<<8) + RGB_result[0]);
	color[1]= ((RGB_result[3]<<8) + RGB_result[2]);
	color[2]= ((RGB_result[5]<<8) + RGB_result[4]);
	color[3]= ((RGB_result[7]<<8) + RGB_result[6]);


	printf("CLEAR = %.2f  " ,color[0]);
	printf("RED = %.2f  "   ,color[1]);
	printf("BLUE = %.2f  "  ,color[2]);
	printf("GREEN = %.2f \n\n" ,color[3]);


	//X_MSB,X_LSB,Y_MSB,Y_LSB,Z_MSB,Z_LSB
	//(void)bbI2CZip(2, MMA_read_buf,sizeof(MMA_read_buf),MMA_result,sizeof(MMA_result));
	

	/***
	//Accel XYZ
	accel[0]= ((MMA_result[0]<<8) + MMA_result[1]);
	accel[1]= ((MMA_result[2]<<8) + MMA_result[3]);
	accel[2]= ((MMA_result[4]<<8) + MMA_result[5]);
	***/

	/***
	printf("MMA_result = %c  "   ,MMA_result[0]);
	printf("MMA_result = %c  "   ,MMA_result[1]);
	printf("MMA_result = %c  "   ,MMA_result[2]);
	printf("MMA_result = %c  "   ,MMA_result[3]);
	printf("MMA_result = %c  "   ,MMA_result[4]);
	printf("MMA_result = %c  "   ,MMA_result[5]);

	printf("X = %.2f  "   ,accel[0]);
	printf("Y = %.2f  "   ,accel[1]);
	printf("Z = %.2f  \n" ,accel[2]);
	***/
}
  bbI2CClose(2);

  gpioTerminate();
}
