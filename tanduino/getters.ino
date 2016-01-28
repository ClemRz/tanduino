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

V getAverageReading(int sensorId) {
  V vector = {0, 0, 0, 0, 0, 0};
  for(int i=0; i<READ_SAMPLES; i++) {
    sumToVector(&vector, getVector(sensorId));
    delay(5);
  }
  divideVector(&vector, READ_SAMPLES);
  switch(sensorId) {
    case ADXL345:
      lowPassFilter(&vector, &_y_ADXL345);
      vector.pitch = getPitch(vector.x, vector.y, vector.z);
      vector.roll = getRoll(vector.y, vector.z);
      break;
    case HMC5883:
      lowPassFilter(&vector, &_y_HMC5883);
      float headingRad = atan2(vector.y, vector.x);
      if(headingRad < 0) headingRad += 2 * M_PI;
      vector.heading = getDegFromRad(headingRad);
      break;
  }
  #if VERBOSE_MODE
  displayVectorValues(vector);
  #endif  //VERBOSE_MODE
  return vector;
}

V getVector(int sensorId) {
  V vector;
  switch(sensorId) {
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
  V vector = {0, 0, 0, 0, 0, 0};
  for(int i=0; i<3; i++) vector.v[i] = sensor.v[i];
  return vector;
}

float getPitch(float x, float y, float z) {
  return getDegFromRad(atan2(x, sqrt(y*y + z*z)));
}

float getRoll(float y, float z) {
  return getDegFromRad(atan2(-y, z));
}

float getDegFromRad(float rad) {
  return rad * 180.0 / M_PI;
}

float getLowPassFiteredValue(float x, float y) {
  return x * ALPHA + (y * (1.0 - ALPHA));
}

