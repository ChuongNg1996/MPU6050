#include "Wire.h"
#include "MPU6050.h"
#include "I2Cdev.h"

// ----------------------- Variables ----------------------- //
int gyro_X, gyro_Y, gyro_Z;
long gyroErr_X, gyroErr_Y, gyroErr_Z;
long acc_X, acc_Y, acc_Z, accVector;
long accErr_X, accErr_Y, accErr_Z;
float roll, pitch, yaw = 0;
int temp;

void setup() {
  Serial.begin(9600);
  Wire.begin();                                                           // Start I2C as master
  MPU6050_Registers();                                                    // Setup the registers of the MPU-6050 
  MPU6050 sensor(0x68);
  MPU6050_Offset();
}
void loop(){
  MPU6050_Angle();
  Serial.println(yaw);
  delay(200);   
}

void MPU6050_Registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        // Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    // Send the requested starting register
  Wire.write(0x00);                                                    // Set the requested starting register
  Wire.endTransmission();                                             
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        // Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    // Send the requested starting register
  Wire.write(0x10);                                                    // Set the requested starting register
  Wire.endTransmission();                                             
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        // Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    // Send the requested starting register
  Wire.write(0x08);                                                    // Set the requested starting register
  Wire.endTransmission();                                             
}
void MPU6050_Data(){                                             // Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        // Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    // Send the requested starting register
  Wire.endTransmission();                                              // End the transmission
  Wire.requestFrom(0x68,14);                                           // Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        // Wait until all the bytes are received
  acc_X = Wire.read()<<8|Wire.read();                                  
  acc_Y = Wire.read()<<8|Wire.read();                                  
  acc_Z = Wire.read()<<8|Wire.read();                                  
  temp = Wire.read()<<8|Wire.read();                                   
  gyro_X = Wire.read()<<8|Wire.read();                                 
  gyro_Y = Wire.read()<<8|Wire.read();                                 
  gyro_Z = Wire.read()<<8|Wire.read();                                 
}
void MPU6050_Offset(){
  for (int i = 0; i < 1000 ; i++){     

    MPU6050_Data();  

    // Accumulate Acceleration Offset  
    accErr_X += acc_X;
    accErr_Y += acc_Y;
    accErr_Z += acc_Z;

    // Accumulate Gyro Offset
    gyroErr_X += gyro_X;                                              
    gyroErr_Y += gyro_Y;                                              
    gyroErr_Z += gyro_Z;  

    delay(3);                                                          
  }

  // Average Acceleration Offset  
  accErr_X /= 1000;
  accErr_Y /= 1000;
  accErr_Z /= 1000;

  // Average Gyro Offset 
  gyroErr_X /= 1000;                                                 
  gyroErr_Y /= 1000;                                                 
  gyroErr_Z /= 1000;
}
void MPU6050_Angle(){
  MPU6050_Data();  
  gyro_X -= gyroErr_X;
  gyro_Y -= gyroErr_Y;
  gyro_Z -= gyroErr_Z;

  roll += gyro_X/500;
  pitch += gyro_Y/500;
  yaw += gyro_Z/500;
}