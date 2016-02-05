void setPCD8544() {
  char buffer[10];
  _PCD8544.clearDisplay();
  _PCD8544.setFont(&PCD8544_FONT);
  _PCD8544.setTextWrap(PCD8544_TEXT_WRAP);
  _PCD8544.setCursor(0,PCD8544_FONT.yAdvance-10);
  if (_y_ADXL345.failed) {
    _PCD8544.print(getError(buffer, ERROR_ADXL345_INIT));
  } else {
    _PCD8544.print(ftoa(buffer, _y_ADXL345.pitch, 1));
  }
  _PCD8544.println();
  if (_y_HMC5883.failed) {
    _PCD8544.print(getError(buffer, ERROR_HMC5883_INIT));
  } else {
    _PCD8544.print(ftoa(buffer, _y_HMC5883.heading, 1));
    _PCD8544.print(F("N"));
  }
  _PCD8544.println();
  _PCD8544.setFont(); //TODO verify if we need to setCursor somewhere, including to put that text at the very bottom of the screen
  _PCD8544.print(NAME); _PCD8544.print(F(" V")); _PCD8544.println(REVISION_NR);
  _PCD8544.display();
}

void setPCD8544Error(String errorMessage) {
  _PCD8544.clearDisplay();
  _PCD8544.setTextWrap(PCD8544_TEXT_WRAP);
  _PCD8544.setCursor(0,12);
  _PCD8544.println(errorMessage);
  _PCD8544.display();
}

