void initPCD8544(void) {
  _PCD8544.begin(PCD8544_CONTRAST, PCD8544_BIAS);
  #if PCD8544_FLIP
  _PCD8544.setRotation(2);
  #endif  //VERBOSE_MODE
  _PCD8544.setFont(&PCD8544_FONT);
  #if VERBOSE_MODE
  Serial.println(F("PCD8544 init"));
  #endif  //VERBOSE_MODE
}

void initADXL345(void) {
  if(!_ADXL345.begin()) {
    //TODO manage errors in LCD
    Serial.println(F("Ooops, no ADXL345 detected ... Check your wiring!"));
    while(1);
  }
  #if VERBOSE_MODE
  sensor_t sensor;
  _ADXL345.getSensor(&sensor);
  displaySensorDetails(sensor);
  displayADXL345DataRate();
  displayADXL345Range();
  #endif  //VERBOSE_MODE
}

void initHMC5883(void) {
  if(!mag.begin()) {
    //TODO manage errors in LCD
    Serial.println(F("Ooops, no HMC5883 detected ... Check your wiring!"));
    while(1);
  }
  #if VERBOSE_MODE
  sensor_t sensor;
  mag.getSensor(&sensor);
  displaySensorDetails(sensor);
  #endif  //VERBOSE_MODE
}
