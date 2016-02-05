void initPCD8544(void) {
  _PCD8544.begin(PCD8544_CONTRAST, PCD8544_BIAS);
  #if FLIP_DISPLAY
  _PCD8544.setRotation(2);
  #endif  //FLIP_DISPLAY
  _PCD8544.display(); //Show splash screen
  delay(1*SEC*MILLISEC);
  #if VERBOSE_MODE
  Serial.println(F("PCD8544 init"));
  #endif  //VERBOSE_MODE
}

void initADXL345(void) {
  if(!_ADXL345.begin()) {
    _y_ADXL345.failed = true;
    return;
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
    _y_HMC5883.failed = true;
    return;
  }
  #if VERBOSE_MODE
  sensor_t sensor;
  mag.getSensor(&sensor);
  displaySensorDetails(sensor);
  #endif  //VERBOSE_MODE
}
