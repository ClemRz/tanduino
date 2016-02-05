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
      _y_ADXL345.pitch = getPitch(_y_ADXL345.x, _y_ADXL345.y, _y_ADXL345.z);
      _y_ADXL345.roll = getRoll(_y_ADXL345.y, _y_ADXL345.z);
      break;
    case HMC5883:
      _y_HMC5883.heading = getHeading(_y_HMC5883.x, _y_HMC5883.y, _y_HMC5883.z, _y_ADXL345.x, _y_ADXL345.y);
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

float getPitch(float x, float y, float z) {
  return rad2Deg(atan2(x, sqrt(y*y + z*z)));
}

float getRoll(float y, float z) {
  return rad2Deg(atan2(-y, z));
}

float getHeading(float cX, float cY, float cZ, float aX, float aY) {
  if (_y_ADXL345.failed) return getNotTiltCompensatedHeading(cX, cY);
  return getTiltCompensatedHeading(cX, cY, cZ, aX, aY);
}

// Inspired from https://github.com/landis/arduino/blob/master/tiltcompass/tiltcompass.pde
float getTiltCompensatedHeading(float cX, float cY, float cZ, float aX, float aY) {
  float
    roll = asin(aY),
    pitch = asin(aX);
  // We cannot correct for tilt over 40 degrees with this algorthem
  if(roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78) return 999.90;
  float
    cosRoll = cos(roll),
    sinRoll = sin(roll),
    cosPitch = cos(pitch),
    sinPitch = sin(pitch),
    hX = cX * cosPitch + cZ * sinPitch,
    hY = cX * sinRoll * sinPitch + cY * cosRoll - cZ * sinRoll * cosPitch;
  return correctedRad2Deg(atan2(hY, hX));
}

float getNotTiltCompensatedHeading(float x, float y) {
  return correctedRad2Deg(atan2(y, x));
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
  return round(mapD(getOutputVoltage(BATT_OUT), BATT_MIN, BATT_MAX, 0.0, 4.0));
}

float getOutputVoltage(int pinToRead) {
  return averageAnalogRead(pinToRead) * 3.3 / averageAnalogRead(REF_3V3); // milivolts
}
