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

S getADXL345(void) {
  sensors_event_t event; 
  _ADXL345.getEvent(&event);
  S sensor = getReadingFromSensor(event.acceleration);
  multiplySensorWithVector(&sensor, _oADXL345);
  return sensor;
}

S getHMC5883(void) {
  sensors_event_t event; 
  mag.getEvent(&event);
  S sensor = getReadingFromSensor(event.magnetic);
  multiplySensorWithVector(&sensor, _oHMC5883);
  sumSensorWithVector(&sensor, _hiHMC5883);
  return sensor;
}

void buildReading(int sensorId) {
  buildAverageAndFilteredReading(sensorId);
  switch (sensorId) {
    case ADXL345:
      _yADXL345.roll = getRoll(_yADXL345.y, _yADXL345.z);
      _yADXL345.pitch = getPitch(_yADXL345.x, _yADXL345.y, _yADXL345.z, _yADXL345.roll);
      break;
    case HMC5883:
      _yHMC5883.heading = getHeading(_yHMC5883.x, _yHMC5883.y, _yHMC5883.z, _yADXL345.roll, _yADXL345.pitch);
      break;
  }
}

void buildAverageAndFilteredReading(int sensorId) {
  S sensor = getAverage(sensorId);
  switch (sensorId) {
    case ADXL345:
      lowPassFilter(&sensor, &_yADXL345);
      break;
    case HMC5883:
      lowPassFilter(&sensor, &_yHMC5883);
      break;
  }
}

S getAverage(int sensorId) {
  S sensor = {0, 0, 0, 0, 0, 0, 0};
  for(int i=0; i<READ_SAMPLES; i++) {
    sum2Sensors(&sensor, getSensor(sensorId));
    delay(5);
  }
  divideSensor(&sensor, READ_SAMPLES);
  return sensor;
}

S getSensor(int sensorId) {
  S sensor;
  switch (sensorId) {
    case ADXL345:
      sensor = getADXL345();
      break;
    case HMC5883:
      sensor = getHMC5883();
      break;
  }
  return sensor;
}

S getReadingFromSensor(sensors_vec_t sensor) {
  S reading = {0, 0, 0, 0, 0, 0, 0};
  for(int i=0; i<3; i++) reading.floatAt[i] = sensor.v[i];
  return reading;
}

// Inspired from https://github.com/adafruit/Adafruit_AHRS/blob/master/Adafruit_Simple_AHRS.cpp
// and http://cache.freescale.com/files/sensors/doc/app_note/AN4248.pdf
float getPitch(float x, float y, float z, float roll) {  
  float pitch, d = y * sin(roll) + z * cos(roll);
  if (d == 0) {
    pitch =  x > 0 ? (-M_PI / 2) : (M_PI / 2);
  } else {
    pitch = (float)atan(-x / d);
  }
  if (pitch > M_PI / 2) pitch = M_PI - pitch;
  if (pitch < -M_PI / 2) pitch = -M_PI - pitch;
  return pitch;
}

// Inspired from http://cache.freescale.com/files/sensors/doc/app_note/AN4248.pdf
float getRoll(float y, float z) {
  return atan2(y, z);
}

float getHeading(float x, float y, float z, float roll, float pitch) {
  if (_yADXL345.failed) return getNotTiltCompensatedHeading(x, y);
  return getTiltCompensatedHeading(x, y, z, roll, pitch);
}

float getTiltCompensatedHeading(float x, float y, float z, float roll, float pitch) {
  float sinRoll = sin(roll),
        cosRoll = cos(roll),
        cosPitch = cos(pitch),
        nz = y * sinRoll + z * cosRoll;
  if (cosPitch < 0) cosPitch *= -1;
  return atan2(nz * sinRoll - y * cosRoll, x * cosPitch + nz * sin(pitch));
}

float getNotTiltCompensatedHeading(float x, float y) {
  return atan2(y, x);
}

char *getError(char* a, uint8_t errorCode) {
  char *ret = a;
  strcpy_P(ret, (char*)pgm_read_word(&(ERROR_MESSAGES[errorCode])));
  #if VERBOSE_MODE
  Serial.print(F("error #"));
  Serial.print(errorCode);
  Serial.print(F(": "));
  Serial.println(ret);
  #endif  //VERBOSE_MODE
  return ret;
}

int getBatteryLevel(void) {
  return round(mapD(getOutputVoltage(BATTERY_PIN)*BATT_DIVIDER, BATT_MIN, BATT_MAX, 0.0, 4.0));
}

float getOutputVoltage(int pinToRead) {
  return averageAnalogRead(pinToRead) * 3.3 / averageAnalogRead(REFERENCE_3V3_PIN); // volts
}
