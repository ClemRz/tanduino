sensors_vec_t getADXL345(void) {
  sensors_event_t event; 
  _ADXL345.getEvent(&event);
  event.acceleration.pitch = getDegFromRad(atan2(event.acceleration.x, event.acceleration.z));
  event.acceleration.roll = getDegFromRad(atan2(event.acceleration.z, event.acceleration.y));
  #if VERBOSE_MODE
  display3Da(event.acceleration);
  #endif  //VERBOSE_MODE
  return event.acceleration;
}

sensors_vec_t getHMC5883(void) {
  sensors_event_t event; 
  mag.getEvent(&event);
  float headingRad = atan2(event.magnetic.y, event.magnetic.x);
  if(headingRad < 0) headingRad += 2 * PI;
  event.magnetic.heading = getDegFromRad(headingRad);
  #if VERBOSE_MODE
  display3Da(event.magnetic);
  #endif  //VERBOSE_MODE
  return event.magnetic;
}

sensors_vec_t getAverageReading(int sensorId) {
  sensors_vec_t runningVector;
  for(int i=0; i<READ_SAMPLES; i++) {
    sumToVector(&runningVector, getVector(sensorId));
    delay(5);
  }
  divideVector(&runningVector, READ_SAMPLES);
  return runningVector;
}

sensors_vec_t getVector(int sensorId) {
  switch(sensorId) {
    case ADXL345:
      return getADXL345();
    case HMC5883:
      return getHMC5883();
  }
}

float getDegFromRad(float rad) {
  return rad * 180.0f / M_PI;
}
