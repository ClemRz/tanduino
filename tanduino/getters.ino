V getADXL345(void) {
  sensors_event_t event; 
  _ADXL345.getEvent(&event);
  V vector = getVectorFromSensor(event.acceleration);
  vector.pitch = getDegFromRad(atan2(vector.x, vector.z));
  vector.roll = getDegFromRad(atan2(vector.z, vector.y));
  #if VERBOSE_MODE
  displayVectorValues(vector);
  #endif  //VERBOSE_MODE
  return vector;
}

V getHMC5883(void) {
  sensors_event_t event; 
  mag.getEvent(&event);
  V vector = getVectorFromSensor(event.magnetic);
  float headingRad = atan2(vector.y, vector.x);
  if(headingRad < 0) headingRad += 2 * M_PI;
  vector.heading = getDegFromRad(headingRad);
  #if VERBOSE_MODE
  displayVectorValues(vector);
  #endif  //VERBOSE_MODE
  return vector;
}

V getAverageReading(int sensorId) {
  V runningVector;
  for(int i=0; i<READ_SAMPLES; i++) {
    sumToVector(&runningVector, getVector(sensorId));
    delay(5);
  }
  divideVector(&runningVector, READ_SAMPLES);
  return runningVector;
}

V getVector(int sensorId) {
  V vector;
  switch(sensorId) {
    case ADXL345:
      vector = getADXL345();
    case HMC5883:
      vector = getHMC5883();
  }
  return vector;
}

V getVectorFromSensor(sensors_vec_t sensor) {
  V vector;
  for(int i=0; i<3; i++) vector.v[i] = sensor.v[i];
  return vector;
}

float getDegFromRad(float rad) {
  return rad * 180.0f / M_PI;
}

