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
uint16_t accel[3];//actual accelerometer reading before normalization
float accel_g[3]; //accelerometer reading after normalization
//function prototype
float MMA_g_out(uint16_t accel_reading);

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
                       2, 7, 2, MMA8451_CTRL_REG1, 0x18, 3,    // put MMA in standby
                       //2, 7, 2, MMA8451_FF_MT_CFG, 0xF8, 3,    // configure interrupt to detect freefall in XYZ direction, event flag latch enabled - read FF_MT_SRC to clear flag
                       //2, 7, 2, 
						2, 7, 2, MMA8451_XYZ_DATA_CFG, 0x02, 3, // Configure XYZ +/- 8g (full-scale  = 8g)
						2, 7, 2, MMA8451_CTRL_REG1, 0x21,3,	//initialize MMA
                       0 // EOL
                       };

char MMA_read_buf[] = {4, MMA8451_ADDR,  // set up Chip address
                       2, 7, 1, MMA8451_OUT_X_MSB, 2, 6, 6, 3,  //read 6 bytes following MMA8451_OUT_X_MSB
                       0 // EOL
                       };

char MMA_check_buf[] = {4, MMA8451_ADDR,  // set up Chip address
                       2, 7, 1, MMA8451_WHO_AM_I ,2,6,1, 3, 
                       0 // EOL
                       };

int main(){

bbI2CClose(2);

gpioTerminate();

while (gpioInitialise() < 0) { //initialization of GPIO library
	static int errcounter = 0;
	errcounter++;
	if (errcounter>50) {
		printf("Pigpio library initialization failed\n");
		return -1;
	}
	
}

while (bbI2COpen(2,3,100000) != 0){ //start Bitbanging on with SDA = pin2, SCL = pin3
	errcounter++;
	if (errcounter>100) {
		errprintf("Bit Banging initialization failed\n");
		return -1;
	}
}

/*** Initialize MMA ***/

err = bbI2CZip(2,MMA_init_buf,sizeof(MMA_init_buf),NULL,0);
while (err != 0){
	err = bbI2CZip(2,MMA_init_buf,sizeof(MMA_init_buf),NULL,0);
	//printf ("MMA init error = %d\n", err);
}
//printf ("MMA init pass\n");

err = bbI2CZip(2,MMA_check_buf,sizeof(MMA_check_buf),MMA_check,sizeof(MMA_check) );
while (err != 1){
	err = bbI2CZip(2,MMA_check_buf,sizeof(MMA_check_buf),MMA_check,sizeof(MMA_check) );
	//printf("MMA check ID error = %d\n", err);
}


if(MMA_check[0] == MMA8451_WHO_AM_I_VALUE){
	//printf("MMA Sensor Connected, ID = %d\n", MMA_check[0]);
}else{
	printf("ERROR! MMA Sensor not found\n");
	while(bbI2CClose(2)!=0);
	gpioTerminate();
	return -1;
}


printf("X, Y, Z \n");

for(;;){
	
	err = bbI2CZip(2, MMA_read_buf,sizeof(MMA_read_buf),MMA_result,sizeof(MMA_result));
	
  //Accel XYZ
	accel[0]= ((MMA_result[0]<<8) + MMA_result[1])&0xFFFC;
	accel[1]= ((MMA_result[2]<<8) + MMA_result[3])&0xFFFC;
	accel[2]= ((MMA_result[4]<<8) + MMA_result[5])&0xFFFC;
 	accel_g[0] = MMA_g_out(accel[0]);
 	accel_g[1] = MMA_g_out(accel[1]);
 	accel_g[2] = MMA_g_out(accel[2]);
  if (err == 6){
	printf("%.4f, "   ,accel_g[0]);
	printf("%.4f, "   ,accel_g[1]);
	printf("%.4f, \n" ,accel_g[2]);
  }
  else{
  //printf("MMA read error. error = %d \n", err);
  }
	
}
  bbI2CClose(2);

  gpioTerminate();
}


float MMA_g_out(uint16_t accel_reading){
	int accel_g_hi = 0;
	int accel_g_low = 0;
   bool negative = 0;
	/*
	** Determine sign and output
	*/
	if ((accel_reading & 0x8000)  == 0x8000){
		accel_reading &= 0xFFFC;
    negative = 1;
		accel_reading = ~accel_reading + 1;
	}
	/*
	** Determine integer value and output
	*/
	accel_g_hi = ((accel_reading>>8) & 0x70) >>4;
	/*
	** Determine mantissa value
	*/
	if ((accel_reading & 0x0800) == 0x0800) {
    accel_g_low += 5000;
  }
	if ((accel_reading & 0x0400) == 0x0400) {

      accel_g_low += 2500;
  }
	if ((accel_reading & 0x0200) == 0x0200) {
      accel_g_low += 1250;
  }
	if ((accel_reading & 0x0100) == 0x0100) {
      accel_g_low += 625;
  }
	if ((accel_reading & 0x0080) == 0x0080) {
      accel_g_low += 313;
  }
	if ((accel_reading & 0x0040) == 0x0040) {
      accel_g_low += 156;
  } 
	if ((accel_reading & 0x0020) == 0x0020) {
      accel_g_low += 78;
  }
	if ((accel_reading & 0x0010) == 0x0010) {
      accel_g_low += 39;
  }
	if ((accel_reading & 0x0008) == 0x0008) {
      accel_g_low += 20;
  }
	if ((accel_reading & 0x0004) == 0x0004) {
      accel_g_low += 10;
  }
	float accel_g_float = (float)accel_g_hi + ((float)accel_g_low)/10000;
 if (negative) accel_g_float = -accel_g_float;
	return accel_g_float;
}