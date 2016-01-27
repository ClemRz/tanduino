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
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "structures.h"

// Debug options
#define VERBOSE_MODE          1               // Echo data to serial port: 0: none, 1 basic info, 2 detailed info, 3 extended info
#define WAIT_TO_START         0               // Wait for serial input in setup()

// General
#define MILLISEC              1000L
#define SEC                   1L
#define MINUTE                (unsigned int) 60L*SEC
#define HOUR                  (unsigned int) 60L*MINUTE
#define DAY                   (unsigned long) 24L*HOUR

// Pins
#define PCD8544_DC_PIN        8               // LCD Data/Command select
#define PCD8544_RST_PIN       9               // LCD Reset
#define PCD8544_CE_PIN        10              // LCD Chip Select

// Sensors settings
#define ADXL345               1               // Accelerometer's ID
#define HMC5883               2               // Compass's ID

// Display settings
#define PCD8544_FLIP          1               // (true, false) flip vertically the display
#define PCD8544_TEXT_WRAP     1               // (true, false) wrap the text
#define PCD8544_CONTRAST      0x31            // LCD Contrast value (0x00 to 0x7F) (the higher the value, the higher the contrast)
#define PCD8544_BIAS          0x13            // LCD Bias mode for MUX rate (0x10 to 0x17) (optimum: 0x13, 1:48)
#define PCD8544_SPI_CLOCK_DIV SPI_CLOCK_DIV2  // Max SPI clock speed for PCD8544 of 2mhz (8mhz / 4)
#define PCD8544_FONT          FreeSans9pt7b   // LCD font
#define FONT_PATH             "Fonts/FreeSans9pt7b.h" // See Fonts folder in Adafruit_GFX library
#include FONT_PATH

// Unit-specific configurations
#define REVISION_NR           F("1.0")        // Revision # of this sketch
#define READ_SAMPLES          8               // Number of samples to average on
#define DISPLAY_REFRESH_RATE  0.175*SEC       // how often display is refreshed (seconds)

// Global variables
Adafruit_PCD8544 _PCD8544 =           Adafruit_PCD8544(PCD8544_DC_PIN, PCD8544_CE_PIN, PCD8544_RST_PIN);
Adafruit_ADXL345_Unified _ADXL345 =   Adafruit_ADXL345_Unified(ADXL345);
Adafruit_HMC5883_Unified mag =        Adafruit_HMC5883_Unified(HMC5883);
static unsigned long _timer = -DISPLAY_REFRESH_RATE*MILLISEC;

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
  unsigned long displayRefreshRate = DISPLAY_REFRESH_RATE*MILLISEC;
  if (millis() - _timer > displayRefreshRate) {
    V v = getAverageReading(ADXL345);
    float pitch = v.pitch;
    float roll = v.roll;
    #if VERBOSE_MODE
    Serial.println(pitch);
    Serial.println(roll);
    #endif  //VERBOSE_MODE
    v = getAverageReading(HMC5883);
    float heading = v.heading;
    #if VERBOSE_MODE
    Serial.println(heading);
    #endif  //VERBOSE_MODE
    setPCD8544(pitch, heading);
    _timer = millis();
  }
}
