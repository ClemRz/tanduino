/**
 * Please read license.txt file before redistributing
 * 
 * Hardware:
 *  - Arduino Pro-Mini 3.3V 8MHz
 *  - Triple Axis Digital Compass HMC5883L
 *  - Triple Axis Accelerometer ADXL345
 *  - 84*48 LCD Display PCD8544
 *  
 */

#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include "PCD8544_SPI.h"

// Debug options
#define VERBOSE_MODE          0               // Echo data to serial port: 0: none, 1 basic info, 2 detailed info, 3 extended info
#define WAIT_TO_START         0               // Wait for serial input in setup()

// General
#define MILLISEC              1000L
#define SEC                   1L
#define MINUTE                (unsigned int) 60L*SEC
#define HOUR                  (unsigned int) 60L*MINUTE
#define DAY                   (unsigned long) 24L*HOUR

// Pins
// TODO define pins here if needed

// Sensors
#define ADXL345               1               // Accelerometer's ID
#define HMC5883               2               // Compass's ID
#define LCD_INVERT            0               // Invert display (true or false)
#define LCD_CONTRAST          0xB1            // Contrast value (0x00 to 0x7FH) (the higher the value, the higher the contrast)
#define LCD_TEMP_COEF         0x04            // Temperature compensation (0x04 to 0x07) (the lower the value, the higher the contrast)
#define LCD_BIAS              0x13            // Bias mode for MUX rate (0x10 to 0x17) (optimum: 0x13, 1:48)

// Unit-specific configurations
#define REVISION_NR           F("1.0")        // Revision # of this sketch
#define READ_SAMPLES          8               // Number of samples to average on

// Global variables
PCD8544_SPI _PCD8544;
Adafruit_ADXL345_Unified _ADXL345 = Adafruit_ADXL345_Unified(ADXL345);
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(HMC5883);

void setup(void) {
  #if VERBOSE_MODE || WAIT_TO_START
  Serial.begin(115200);
  Serial.println();
  #endif  //VERBOSE_MODE || WAIT_TO_START
  #if VERBOSE_MODE
  displaySketchInfo();
  #endif  //VERBOSE_MODE
  #if WAIT_TO_START
  Serial.println(F("Type any character to start"));
  while (!Serial.available());
  #endif //WAIT_TO_START
  initPCD8544();
  initADXL345();
  initHMC5883();
}

void loop(void) {
  float inclination = averageRead(ADXL345);
  #if VERBOSE_MODE
  Serial.println(inclination);
  #endif  //VERBOSE_MODE
  float azimut = averageRead(HMC5883);
  #if VERBOSE_MODE
  Serial.println(azimut);
  #endif  //VERBOSE_MODE
  setPCD8544(inclination, azimut);
  delay(0.175*SEC*MILLISEC);
}

void initPCD8544(void) {
  _PCD8544.begin(LCD_INVERT, LCD_CONTRAST, LCD_TEMP_COEF, LCD_BIAS);
  #if VERBOSE_MODE
  Serial.println(F("PCD8544 init"));
  #endif  //VERBOSE_MODE
}

void initADXL345(void) {
  if(!_ADXL345.begin()) {
    //TODO manage errors in LCD
    Serial.println(F("Ooops, no ADXL345 detected ... Check your wiring!"));
    while(1);
  }
  #if VERBOSE_MODE
  sensor_t sensor;
  _ADXL345.getSensor(&sensor);
  displaySensorDetails(sensor);
  displayADXL345DataRate();
  displayADXL345Range();
  #endif  //VERBOSE_MODE
}

void initHMC5883(void) {
  if(!mag.begin()) {
    //TODO manage errors in LCD
    Serial.println(F("Ooops, no HMC5883 detected ... Check your wiring!"));
    while(1);
  }
  #if VERBOSE_MODE
  sensor_t sensor;
  mag.getSensor(&sensor);
  displaySensorDetails(sensor);
  #endif  //VERBOSE_MODE
}

void setPCD8544(float val1, float val2) {
  _PCD8544.clear();
  _PCD8544.print(val1);
  _PCD8544.print(F(" deg."));
  _PCD8544.gotoXY(0,2);
  _PCD8544.print(val2);
  _PCD8544.print(F(" deg. N"));
}

float getADXL345(void) {
  sensors_event_t event; 
  _ADXL345.getEvent(&event);
  float inclination = rad2deg(atan2(event.acceleration.x, event.acceleration.z));
  #if VERBOSE_MODE
  display3Da(event.acceleration, inclination);
  #endif  //VERBOSE_MODE
  return inclination;
}

float getHMC5883(void) {
  sensors_event_t event; 
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  if(heading < 0) heading += 2*PI;
  float azimut = rad2deg(heading);
  #if VERBOSE_MODE
  display3Da(event.magnetic, azimut);
  #endif  //VERBOSE_MODE
  return azimut;
}

float averageRead(int sensorId) {
  float runningValue = 0.0f; 
  for(int i=0; i<READ_SAMPLES; i++) {
    switch(sensorId) {
      case ADXL345:
        runningValue += getADXL345();
        break;
      case HMC5883:
        runningValue += getHMC5883();
        break;
    }
    delay(5);
  }
  return runningValue / READ_SAMPLES;
}

float rad2deg(float angle) {
  return angle * 180.0f / M_PI;
}

#if VERBOSE_MODE
void display3Da(sensors_vec_t vector, float angle) {
  #if VERBOSE_MODE > 2
  Serial.print("X: "); Serial.print(vector.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(vector.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(vector.z); Serial.print("  ");
  #endif  //VERBOSE_MODE
  #if VERBOSE_MODE > 1
  Serial.print("a: "); Serial.print(angle); Serial.print("  ");Serial.println("deg. ");
  #endif  //VERBOSE_MODE
}
String getSketchName(void) {
  String path = F(__FILE__);
  int slash_loc = path.lastIndexOf('/');
  String the_cpp_name = path.substring(slash_loc+1);
  int dot_loc = the_cpp_name.lastIndexOf('.');
  return the_cpp_name.substring(0, dot_loc);
}
void displaySketchInfo(void) {
  Serial.print(F("["));
  Serial.print(getSketchName());
  Serial.print(F(".ino] Rev. "));
  Serial.println(REVISION_NR);
}
void displaySensorDetails(sensor_t sensor) {
  Serial.println("------------------------------------");
  Serial.print("Sensor:       "); Serial.println(sensor.name);
  Serial.print("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
}
void displayADXL345DataRate(void) {
  Serial.print("Data Rate:    "); 
  
  switch(_ADXL345.getDataRate()) {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print("0.10 "); 
      break;
    default:
      Serial.print("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}
void displayADXL345Range(void) {
  Serial.print("Range:         +/- "); 
  
  switch(_ADXL345.getRange()) {
    case ADXL345_RANGE_16_G:
      Serial.print("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print("2 "); 
      break;
    default:
      Serial.print("?? "); 
      break;
  }  
  Serial.println(" g");  
}
#endif  //VERBOSE_MODE
