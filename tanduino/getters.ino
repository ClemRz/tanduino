V getADXL345(void) {
  sensors_event_t event; 
  _ADXL345.getEvent(&event);
  return getVectorFromSensor(event.acceleration);
}

V getHMC5883(void) {
  sensors_event_t event; 
  mag.getEvent(&event);
  return getVectorFromSensor(event.magnetic);
}

void buildReading(int sensorId) {
  buildAverageAndFilteredReading(sensorId);
  switch (sensorId) {
    case ADXL345:
      _y_ADXL345.roll = getRoll(_y_ADXL345.y, _y_ADXL345.z);
      _y_ADXL345.pitch = getPitch(_y_ADXL345.x, _y_ADXL345.y, _y_ADXL345.z, _y_ADXL345.roll);
      break;
    case HMC5883:
      _y_HMC5883.heading = getHeading(_y_HMC5883.x, _y_HMC5883.y, _y_HMC5883.z, _y_ADXL345.roll, _y_ADXL345.pitch);
      break;
  }
}

void buildAverageAndFilteredReading(int sensorId) {
  V vector = getAverage(sensorId);
  switch (sensorId) {
    case ADXL345:
      lowPassFilter(&vector, &_y_ADXL345);
      break;
    case HMC5883:
      lowPassFilter(&vector, &_y_HMC5883);
      break;
  }
}

V getAverage(int sensorId) {
  V vector = {0, 0, 0, 0, 0, 0, 0};
  for(int i=0; i<READ_SAMPLES; i++) {
    sumToVector(&vector, getVector(sensorId));
    delay(5);
  }
  divideVector(&vector, READ_SAMPLES);
  return vector;
}

V getVector(int sensorId) {
  V vector;
  switch (sensorId) {
    case ADXL345:
      vector = getADXL345();
      break;
    case HMC5883:
      vector = getHMC5883();
      break;
  }
  return vector;
}

V getVectorFromSensor(sensors_vec_t sensor) {
  V vector = {0, 0, 0, 0, 0, 0, 0};
  for(int i=0; i<3; i++) vector.v[i] = sensor.v[i];
  return vector;
}

// Inspired from https://github.com/adafruit/Adafruit_AHRS/blob/master/Adafruit_Simple_AHRS.cpp
float getPitch(float x, float y, float z, float roll) {
  float pitch;
  if (z * cos(roll) - y * sin(roll) == 0) {
    pitch =  x > 0 ? (-M_PI / 2) : (M_PI / 2);
  } else {
    pitch = (float)atan(x / (z * cos(roll) - y * sin(roll)));
  }
  return pitch;
}

float getRoll(float y, float z) {
  return -atan2(y, z);
}

float getHeading(float x, float y, float z, float roll, float pitch) {
  if (_y_ADXL345.failed) return getNotTiltCompensatedHeading(x, y);
  return getTiltCompensatedHeading(x, y, z, roll, pitch);
}

float getTiltCompensatedHeading(float x, float y, float z, float roll, float pitch) {
  return atan2(z * sin(roll) + y * cos(roll), x * cos(pitch) + y * sin(pitch) * sin(roll) - z * sin(pitch) * cos(roll));
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
