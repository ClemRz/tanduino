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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "structures.h"

// Debug options
#define VERBOSE_MODE                  0                       // Echo data to serial port: 0: none, 1 basic info, 2 detailed info, 3 extended info
#define WAIT_TO_START                 0                       // Wait for serial input in setup()

// General
#define MILLISEC                      1000L
#define SEC                           1L
#define MINUTE                        (unsigned int)60L*SEC
#define HOUR                          (unsigned int)60L*MINUTE
#define DAY                           (unsigned long)24L*HOUR

// Error codes
#define ERROR_ADXL345_INIT            1
#define ERROR_HMC5883_INIT            2
const char PROGMEM                                            //Remember to update the buffer when adding a larger string [7]
  errmsg_0[] =                        "",                     //SPARE
  errmsg_1[] =                        "Ac err",               //Accelerometer init error
  errmsg_2[] =                        "Co err";               //Compass init error
const char* const PROGMEM        
  ERROR_MESSAGES[] =                  {errmsg_0, errmsg_1, errmsg_2};

// Pins        
#define REFERENCE_3V3_PIN             A0                      // 3.3V reference for sampling
#define BATTERY_PIN                   A1                      // Battery sensing
#define BUTTON_PIN                    2                       // Interrupt pin
#define PIEZO_PIN                     3                       // Piezo speaker/buzzer pin
#define LASER_PIN                     7                       // Laser pointer
#define PCD8544_DC_PIN                8                       // LCD Data/Command select
#define PCD8544_RST_PIN               9                       // LCD Reset
#define PCD8544_CE_PIN                10                      // LCD Chip Select

// Sensors settings
#define ADXL345                       1                       // Accelerometer's ID
#define ADXL345_MEASURE               0x08                    // Value to put POWER_CTL register to measurement value
#define ADXL345_STANDBY               0x00                    // Value to put POWER_CTL register to standby value
#define HMC5883                       2                       // Compass's ID
#define HMC5883_MEASURE               0x00                    // Value to put MR register to continuous measurement value
#define HMC5883_IDLE                  0x02                    // Value to put MR register to idle value
#define READ_SAMPLES                  5.0                     // Number of samples to compute reading average
#define ALPHA                         0.7                     // Low Pass Filter constant
#define CAPTURING_SAMPLES             10                      // Number of samples for stabilization computation             
#define MAXIMUM_DEVIATION             0.2                     // Maximum deviation goal for data capturing (degrees)
#define STABILIZATION_TIMEOUT         2*SEC                   // Maximum allowed time to obtain a satisfactory deviation (seconds)

// Battery settings        
#define BATT_MIN                      7.0                     // Minimum voltage delivered by the battery (volts)
#define BATT_MAX                      9.0                     // Maximum voltage delivered by the battery (volts)
#define BATT_DIVIDER                  1.47/0.47               // Inverse ratio of the voltage divider that gives battery input ( (R1 + Rs) / Rs )
#define BATT_REFRESH_RATE             5*SEC                   // how often battery measurement is refreshed (seconds)

// Button settings        
#define DEBOUNCE_DELAY                50                      // Delay during which we ignore the button actions (milliseconds)
#define LONG_PUSH_DELAY               2*SEC                   // Delay to detect a long push versus a short push (seconds)

// Buzzer settings
#define FREQUENCY                     4100                    // Sound frequency (hertz)
#define SHORT_CHIRP                   50                      // Length of the short chirp (milliseconds)
#define LONG_CHIRP                    250                     // Length of the long chirp (milliseconds)

// Display settings        
#define PCD8544_CONTRAST              50                      // LCD Contrast value
#define PCD8544_SPI_CLOCK_DIV         SPI_CLOCK_DIV2          // Max SPI clock speed for PCD8544 of 2mhz (8mhz / 4)
#define PCD8544_REFRESH_RATE          0.175*SEC               // how often display is refreshed (seconds)
#define PCD8544_TEXT_WRAP             0                       // (true, false) wrap the text
#define PCD8544_FLIP                  1                       // (true, false) flip vertically the display

// Device settings
#define REVISION_NR                   F("1.0")                // Revision # of the design

// Calibration settings
#define BAUD                          9600                    // Serial communication baud for debug and calibration purposes
#define PITCH_0                       0.0942478               // Calibration pitch (rad) used to correct the offset between the horizontal plane and the sensors xy plane. Impact display only. Modify it as needed.
static const V
  _orientationADXL345 =               {-1, 1, 1},             // {x, y, z} Configuration vector to align the sensor output with the device coordinate system (NED: North, East, Down). Modify it as needed.
  _orientationHMC5883 =               {1, -1, -1},            // {x, y, z} Configuration vector to align the sensor output with the device coordinate system (NED: North, East, Down). Modify it as needed.
  _biasHMC5883 =                      {18.95, 7.244, 5.401};  // {x, y, z} Calibration vector used to correct the Hard Iron effect. Modify it as needed.
static const M
  _matrixHMC5883 =                    {1.442, -0.062, -0.053, // Calibration matrix used to correct the Soft Iron effect. Modify it as needed.
                                       -0.032, 1.497, 0.055,  // Theory arround calibration: http://www.nxp.com/files/sensors/doc/app_note/AN4246.pdf
                                       -0.117, 0.052, 1.554};

// Global variables
float
  _circularPitch[CAPTURING_SAMPLES],
  _circularHeading[CAPTURING_SAMPLES];
int _circularIndex =                  0;
Adafruit_PCD8544 _PCD8544 =           Adafruit_PCD8544(PCD8544_DC_PIN, PCD8544_CE_PIN, PCD8544_RST_PIN);
Adafruit_ADXL345_Unified _ADXL345 =   Adafruit_ADXL345_Unified(ADXL345);
Adafruit_HMC5883_Unified mag =        Adafruit_HMC5883_Unified(HMC5883);
static unsigned long
  _timer =                            -PCD8544_REFRESH_RATE*MILLISEC,
  _battTimer =                        -BATT_REFRESH_RATE*MILLISEC;
int _batt =                           0;
bool _calibrate =                     0,
     _hold =                          0,
     _standby =                       0,
     _serial =                        0,
     _circularLooped =                0,
     _beep =                          1;
S
  _yADXL345 =                         {0, 0, 0, 0, 0, 0, 0},
  _yHMC5883 =                         {0, 0, 0, 0, 0, 0, 0};
volatile unsigned long
  _v_lastInterruptTime =              0,
  _v_lowTime =                        0,
  _v_stabilizationTimer =             0;
volatile bool
  _v_toggle =                         0;
volatile uint8_t
  _v_buttonState =                    HIGH;

void setup(void) {
  initSerial();
  #if VERBOSE_MODE
  displaySketchInfo();
  #endif  //VERBOSE_MODE
  #if WAIT_TO_START
  Serial.println(F("Type any character to start"));
  while (!Serial.available());
  #endif //WAIT_TO_START
  initBuzzer();
  initButton();
  initBatt();
  initPCD8544();
  initADXL345();
  initHMC5883();
  initLaser();
}

void loop(void) {
  bool refresh = millis() - _timer > (unsigned long)PCD8544_REFRESH_RATE*MILLISEC;
  toogleSensors();
  if (!_hold && (_v_toggle || refresh)) buildSensorReadings();
  if (_v_toggle) toggleMeasurementMode();
  if(_v_buttonState == LOW && millis() - _v_lowTime > (unsigned long)LONG_PUSH_DELAY*MILLISEC) toggleCalibrationMode();
  if (millis() - _battTimer > (unsigned long)BATT_REFRESH_RATE*MILLISEC) refreshBattery();
  if (refresh) refreshDisplay();
}

void toogleSensors(void) {
  if (_standby != _hold) {
    if (_hold) {
      setADXL345ToStandbyMode();
      setHMC5883ToStandbyMode();
    } else {
      setADXL345ToMeasureMode();
      setHMC5883ToMeasureMode();
    }
    _standby = _hold;
  }
}

void buildSensorReadings(void) {
  if (!_yADXL345.failed) buildSensorReading(ADXL345);
  if (!_yHMC5883.failed) buildSensorReading(HMC5883);
}

void toggleMeasurementMode(void) {
  if (_beep || _hold) {
    shortChirp();
    _beep = false;
  }
  if (_hold) {
    _hold = false;
    _v_toggle = false;
    _beep = true;
  } else {
    consolidateCircularArrays();
    if (isStabilized()) {
      _hold = true;
      _v_toggle = false;
      shortChirp();
    } else if (millis() - _v_stabilizationTimer > (unsigned long)STABILIZATION_TIMEOUT*MILLISEC) {
      _v_toggle = false;
      longChirp();
    }
  }
}

void toggleCalibrationMode(void) {
  _v_buttonState = HIGH;
  _calibrate = !_calibrate;
  longChirp();
  initSerial();
}

void refreshBattery(void) {
  buildBatteryReading();
  _battTimer = millis();
}

void refreshDisplay(void) {
  printHMC5883();
  setLaserStatus();
  setPCD8544();
  _timer = millis();
}

