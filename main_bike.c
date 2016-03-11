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

char RGB_result[8+1]; // for some reason, adding 1 works
char MMA_result[6+1];
char RGB_check[1+1];
char MMA_check[1+1];

int err; //error code

float color[4];//actual color reading
float accel[3];//actual accelerometer reading


char RGB_init_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ATIME), 0xFF, 3,   // Integration of Sensor. 0 - slowest , 0xFF -fastest
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_CONFIG), 0x00, 3, // 
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_CONTROL), 0x01, 3, // 
		                   2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ENABLE), TCS34725_ENABLE_PON,3,	//power on
                       0 // EOL
                       };


char RGB_enable_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 2, (RGB_COMMAND_BIT | TCS34725_ENABLE), (TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN), 3,   // 
                       0 // EOL
                       };

char RGB_check_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 1, (RGB_COMMAND_BIT | TCS34725_ID),2,6,1, 3, 
                       0 // EOL
                       };
                       
char RGB_read_buf[] = {4, RGB_ADDR,  // set up Chip address
                       2, 7, 1, (RGB_COMMAND_BIT | CLEAR_L),2,6,8, 3, 
                       0 // EOL
                       };
                       
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

char MMA_check_buf[] = {4, MMA8451_ADDR,  // set up Chip address
                       2, 7, 1, MMA8451_WHO_AM_I ,2,6,1, 3, 
                       0 // EOL RRF
                       };

int main(){

bbI2CClose(2);

gpioTerminate();

if (gpioInitialise() < 0) { //initialization of GPIO library
	printf("Pigpio library initialization failed\n");
	return -1;
}

if (bbI2COpen(2,3,100000) != 0){ //start Bitbanging on with SDA = pin2, SCL = pin3
	printf("Bit Banging initialization failed\n");
	return -1;
}
/*** Initialize RGB ***/
/*
err = bbI2CZip(2,RGB_init_buf,sizeof(RGB_init_buf),NULL,0);
while (err != 0){
err = bbI2CZip(2,RGB_init_buf,sizeof(RGB_init_buf),NULL,0);
printf ("RGB init error = %d\n", err);
}
printf ("RGB init pass\n");

for (int i=0; i<1000;i++){};

err = bbI2CZip(2,RGB_enable_buf,sizeof(RGB_enable_buf),NULL,0);
while (err != 0){
err = bbI2CZip(2,RGB_enable_buf,sizeof(RGB_enable_buf),NULL,0);
printf ("RGB enable error = %d\n", err);
}
printf ("RGB enable pass\n");


err = bbI2CZip(2,RGB_check_buf,sizeof(RGB_check_buf),RGB_check,sizeof(RGB_check) );
while (err != 1){
err = bbI2CZip(2,RGB_check_buf,sizeof(RGB_check_buf),RGB_check,sizeof(RGB_check) );
printf("RGB check ID error = %d\n", err);
}


if(RGB_check[0] == 0x44){
	printf("RGB Sensor Connected, ID = %d\n", RGB_check[0]);
}else{
	printf("ERROR! RGB Sensor not found\n");
	while(bbI2CClose(2)!=0);
	gpioTerminate();
	return -1;
}
*/

while (gpioInitialise() < 0)
{
  // Failed init
  printf ("GPIO init error = %d\n", err);
}
// Successfully initialized gpio

/*** Initialize MMA ***/

err = bbI2CZip(2,MMA_init_buf,sizeof(MMA_init_buf),NULL,0);
while (err != 0){
err = bbI2CZip(2,MMA_init_buf,sizeof(MMA_init_buf),NULL,0);
printf ("MMA init error = %d\n", err);
// no return cause it keeps trying until it works
}
printf ("MMA init pass\n");



err = bbI2CZip(2,MMA_check_buf,sizeof(MMA_check_buf),MMA_check,sizeof(MMA_check) );
while (err != 1){
err = bbI2CZip(2,MMA_check_buf,sizeof(MMA_check_buf),MMA_check,sizeof(MMA_check) );
printf("MMA check ID error = %d\n", err);
}


if(MMA_check[0] == MMA8451_WHO_AM_I_VALUE){
	printf("MMA Sensor Connected, ID = %d\n", MMA_check[0]);
}else{
	printf("ERROR! MMA Sensor not found\n");
	while(bbI2CClose(2)!=0);
	gpioTerminate();
	return -1;
}



for(;;){
/*
	//Clear, Red, BLue, Green
	err = bbI2CZip(2, RGB_read_buf,sizeof(RGB_read_buf),RGB_result,sizeof(RGB_result));

	//Color_RGB
	color[0]= ((RGB_result[1]<<8) + RGB_result[0]);
	color[1]= ((RGB_result[3]<<8) + RGB_result[2]);
	color[2]= ((RGB_result[5]<<8) + RGB_result[4]);
	color[3]= ((RGB_result[7]<<8) + RGB_result[6]);

  if (err == 8){
	printf("%.2f ," ,color[0]); // CLEAR 
	printf("%.2f ,"   ,color[1]); // RED
	printf("%.2f ,"  ,color[2]); // GREEN
	printf("%.2f ," ,color[3]); // BLUE
 }else{
 printf("RGB read error. error = %d \n", err);
 }
*/

	//X_MSB,X_LSB,Y_MSB,Y_LSB,Z_MSB,Z_LSB
	err = bbI2CZip(2, MMA_read_buf,sizeof(MMA_read_buf),MMA_result,sizeof(MMA_result));
	
  //Accel XYZ
  
	accel[0]= ((MMA_result[0]<<8) + MMA_result[1]);
	accel[1]= ((MMA_result[2]<<8) + MMA_result[3]);
	accel[2]= ((MMA_result[4]<<8) + MMA_result[5]);

 
  if (err == 6){
	printf("%.2f ,"   ,accel[0]); // X
	printf("%.2f ,"   ,accel[1]); // Y
	printf("%.2f , \n" ,accel[2]); // Z
  }
  else{
  printf("MMA read error. error = %d \n", err);
  }
  static bool Toggle = true;
  if (Toggle){
    gpioWrite_Bits_0_31_Clear(4);
  }
  else{
    gpioWrite_Bits_0_31_Set(4);
  }
  Toggle = !Toggle;
	
}
  bbI2CClose(2);

  gpioTerminate();
}
