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

/**
 * TODOs:
 *  - add one buzzer with small chirp on button push and long chirp on power/shut down.
 *  - add one push button to (http://playground.arduino.cc/Learning/ArduinoSleepCode):
 *    . turn on device (long push, attach an interrupt to count millis and go back to sleep if not enough)
 *    . turn off device (same as previous, but instead of going to sleep if not enough, just toggle hold function)
 *    . hold a measurement
 *    . unhold a measurement
 *  - display the degree sign (TBV)
 *  - inclulde the font file in the repo
 *  - measure and display the battery level (TBV)
 *  - warn about excessive roll when unhold
 *  - evaluate the battery consumption
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "structures.h"

// Debug options
#define VERBOSE_MODE          0               // Echo data to serial port: 0: none, 1 basic info, 2 detailed info, 3 extended info
#define WAIT_TO_START         0               // Wait for serial input in setup()

// General
#define MILLISEC              1000L
#define SEC                   1L
#define MINUTE                (unsigned int) 60L*SEC
#define HOUR                  (unsigned int) 60L*MINUTE
#define DAY                   (unsigned long) 24L*HOUR

// Error codes [TBV]
#define ERROR_ADXL345_INIT    1
#define ERROR_HMC5883_INIT    2
const char PROGMEM            //Remember to update the buffer when adding a larger string [10]
  errmsg_0[] =                "",  //SPARE
  errmsg_1[] =                "Ac err", //Accelerometer init error
  errmsg_2[] =                "Co err"; //Compass init error
const char* const PROGMEM
  ERROR_MESSAGES[] =          {errmsg_0, errmsg_1, errmsg_2};

// Pins
#define REF_3V3               A0              // 3.3V reference sampling
#define BATT_OUT              A1              // Battery voltage
#define PCD8544_DC_PIN        8               // LCD Data/Command select
#define PCD8544_RST_PIN       9               // LCD Reset
#define PCD8544_CE_PIN        10              // LCD Chip Select

// Sensors settings
#define ADXL345               1               // Accelerometer's ID
#define HMC5883               2               // Compass's ID
#define ALPHA                 0.5             // Low Pass Filter constant
#define ANALOG_READ_SAMPLES   8.0             // Number of samples to compute analog reading average

// Battery settings
#define BATT_MIN              0.0 //3.4             // Maximum voltage delivered by the battery (volts)
#define BATT_MAX              3.3 //4.0             // Maximum voltage delivered by the battery (volts)

// Display settings
#define PCD8544_TEXT_WRAP     1               // (true, false) wrap the text
#define PCD8544_CONTRAST      0x31            // LCD Contrast value (0x00 to 0x7F) (the higher the value, the higher the contrast)
#define PCD8544_BIAS          0x13            // LCD Bias mode for MUX rate (0x10 to 0x17) (optimum: 0x13, 1:48)
#define PCD8544_SPI_CLOCK_DIV SPI_CLOCK_DIV2  // Max SPI clock speed for PCD8544 of 2mhz (8mhz / 4)

// Unit-specific configurations
#define REVISION_NR           F("1.0")        // Revision # of this sketch
#define FLIP_DISPLAY          0               // (true, false) flip vertically the display
#define SHOW_SPLASH_SCREEN    1               // Show splash screen during initialisation of the device
#define READ_SAMPLES          8               // Number of samples to average on
#define DISPLAY_REFRESH_RATE  0.175*SEC       // how often display is refreshed (seconds)
#define BATT_REFRESH_RATE     5*SEC           // how often battery measurement is refreshed (seconds)

// Global variables
Adafruit_PCD8544 _PCD8544 =           Adafruit_PCD8544(PCD8544_DC_PIN, PCD8544_CE_PIN, PCD8544_RST_PIN);
Adafruit_ADXL345_Unified _ADXL345 =   Adafruit_ADXL345_Unified(ADXL345);
Adafruit_HMC5883_Unified mag =        Adafruit_HMC5883_Unified(HMC5883);
static unsigned long
  _timer =                            -DISPLAY_REFRESH_RATE*MILLISEC,
  _battTimer =                        -BATT_REFRESH_RATE*MILLISEC;
int _batt = 0;
V
  _y_ADXL345 =                        {0, 0, 0, 0, 0, 0, 0},
  _y_HMC5883 =                        {0, 0, 0, 0, 0, 0, 0};

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
  initBatt();
  initPCD8544();
  initADXL345();
  initHMC5883();
}

void loop(void) {
  if (millis() - _battTimer > (unsigned long)BATT_REFRESH_RATE*MILLISEC) {
    _batt = getBatteryLevel();
    _battTimer = millis();
  }
  if (millis() - _timer > (unsigned long)DISPLAY_REFRESH_RATE*MILLISEC) {
    if (!_y_ADXL345.failed) {
      buildReading(ADXL345);
    }
    if (!_y_HMC5883.failed) {
      buildReading(HMC5883);
    }
    setPCD8544();
    _timer = millis();
  }
}
