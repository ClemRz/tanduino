/**
Copyright (c) 2016, Clément Ronzon
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of tanduino nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * TODOs:
 *  - warn about excessive roll when unhold
 *  - define a Soft Iron calibration 3x3 matrix and the logic for Soft Iron effect correction
 *  - calibration: http://www.nxp.com/files/sensors/doc/app_note/AN4246.pdf
 *  - add calibration method to the documentation
 *  - add a picture of the prototype to the documentation
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
#define VERBOSE_MODE                  0               // Echo data to serial port: 0: none, 1 basic info, 2 detailed info, 3 extended info
#define WAIT_TO_START                 0               // Wait for serial input in setup()

// General
#define MILLISEC                      1000L
#define SEC                           1L
#define MINUTE                        (unsigned int)60L*SEC
#define HOUR                          (unsigned int)60L*MINUTE
#define DAY                           (unsigned long)24L*HOUR

// Error codes
#define ERROR_ADXL345_INIT            1
#define ERROR_HMC5883_INIT            2
const char PROGMEM                                    //Remember to update the buffer when adding a larger string [7]
  errmsg_0[] =                        "",             //SPARE
  errmsg_1[] =                        "Ac err",       //Accelerometer init error
  errmsg_2[] =                        "Co err";       //Compass init error
const char* const PROGMEM        
  ERROR_MESSAGES[] =                  {errmsg_0, errmsg_1, errmsg_2};

// Pins        
#define REFERENCE_3V3_PIN             A0              // 3.3V reference for sampling
#define BATTERY_PIN                   A1              // Battery sensing
#define BUTTON_PIN                    2               // Interrupt pin
#define LASER_PIN                     7               // Laser pointer
#define PCD8544_DC_PIN                8               // LCD Data/Command select
#define PCD8544_RST_PIN               9               // LCD Reset
#define PCD8544_CE_PIN                10              // LCD Chip Select

// Sensors settings        
#define ADXL345                       1               // Accelerometer's ID
#define HMC5883                       2               // Compass's ID
#define ALPHA                         0.5             // Low Pass Filter constant
#define READ_SAMPLES                  8.0             // Number of samples to compute reading average

// Battery settings        
#define BATT_MIN                      7.0             // Minimum voltage delivered by the battery (volts)
#define BATT_MAX                      9.0             // Maximum voltage delivered by the battery (volts)
#define BATT_DIVIDER                  1.47/0.47       // Inverse ratio of the voltage divider that gives battery input ( (R1 + Rs) / Rs )
#define BATT_REFRESH_RATE             5*SEC           // how often battery measurement is refreshed (seconds)

// Button settings        
#define DEBOUNCE_DELAY                50              // Delay during which we ignore the button actions (milliseconds)
#define LONG_PUSH_DELAY               1*SEC           // Delay to detect a long push versus a short push (seconds)

// Display settings        
#define PCD8544_CONTRAST              50              // LCD Contrast value
#define PCD8544_SPI_CLOCK_DIV         SPI_CLOCK_DIV2  // Max SPI clock speed for PCD8544 of 2mhz (8mhz / 4)
#define PCD8544_REFRESH_RATE          0.175*SEC       // how often display is refreshed (seconds)
#define PCD8544_TEXT_WRAP             0               // (true, false) wrap the text
#define PCD8544_FLIP                  0               // (true, false) flip vertically the display

// Device settings        
#define REVISION_NR                   F("1.0")        // Revision # of the design

// Calibration constants
static const V
  _orientationADXL345 =               {-1, 1, 1},     // {x, y, z} Configuration vector to align the sensor output with the device coordinate system (NED: North, East, Down). Modify it as needed.
  _orientationHMC5883 =               {1, -1, -1},    // {x, y, z} Configuration vector to align the sensor output with the device coordinate system (NED: North, East, Down). Modify it as needed.
  _biasHMC5883 =                      {0, 0, 0};      // {x, y, z} Calibration vector used to correct the Hard Iron effect. Modify it as needed.
static const M
  _matrixHMC5883 =                    {1, 0, 0,       // Calibration matrix used to correct the Soft Iron effect. Modify it as needed.
                                       0, 1, 0,
                                       0, 0, 1};
static const float _pitch0 =          0;              // Calibration pitch (rad) used to correct the offset between the horizontal plane and the sensors xy plane. Impact display only. Modify it as needed.

// Global variables
Adafruit_PCD8544 _PCD8544 =           Adafruit_PCD8544(PCD8544_DC_PIN, PCD8544_CE_PIN, PCD8544_RST_PIN);
Adafruit_ADXL345_Unified _ADXL345 =   Adafruit_ADXL345_Unified(ADXL345);
Adafruit_HMC5883_Unified mag =        Adafruit_HMC5883_Unified(HMC5883);
static unsigned long
  _timer =                            -PCD8544_REFRESH_RATE*MILLISEC,
  _battTimer =                        -BATT_REFRESH_RATE*MILLISEC;
int _batt =                           0;
bool _calibrate =                     0;
S
  _yADXL345 =                         {0, 0, 0, 0, 0, 0, 0},
  _yHMC5883 =                         {0, 0, 0, 0, 0, 0, 0};
volatile unsigned long
  _v_lastInterruptTime =              0,
  _v_lowTime =                        0;
volatile bool
  _v_hold =                           0,
  _v_buttonState =                    1;

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
  initButton();
  initBatt();
  initPCD8544();
  initADXL345();
  initHMC5883();
  initLaser();
}

void loop(void) {
  if(!_v_buttonState && millis() - _v_lowTime > (unsigned long)LONG_PUSH_DELAY*MILLISEC) {
    _v_buttonState = true;
    _calibrate = !_calibrate;
  }
  if (millis() - _battTimer > (unsigned long)BATT_REFRESH_RATE*MILLISEC) {
    _batt = getBatteryLevel();
    _battTimer = millis();
  }
  if (millis() - _timer > (unsigned long)PCD8544_REFRESH_RATE*MILLISEC) {
    if (!_v_hold) {
      if (!_yADXL345.failed) buildReading(ADXL345);
      if (!_yHMC5883.failed) buildReading(HMC5883);
    }
    setLaserStatus();
    setPCD8544();
    _timer = millis();
  }
}
