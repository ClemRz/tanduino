void setPCD8544(void) {
  char buffer[7];
  _PCD8544.clearDisplay();
  _PCD8544.setTextWrap(PCD8544_TEXT_WRAP);
  _PCD8544.setTextSize(2);
  _PCD8544.setTextColor(BLACK);
  if (_y_ADXL345.failed) {
    _PCD8544.print(getError(buffer, ERROR_ADXL345_INIT));
  } else {
    _PCD8544.print(ftoa(buffer, rad2Deg(_y_ADXL345.pitch), 1)); _PCD8544.write(0xF7);
  }
  _PCD8544.println();
  if (_y_HMC5883.failed) {
    _PCD8544.print(getError(buffer, ERROR_HMC5883_INIT));
  } else {
    _PCD8544.print(ftoa(buffer, correctedRad2Deg(_y_HMC5883.heading), 1)); _PCD8544.write(0xF7); _PCD8544.print('N');
  }
  _PCD8544.println();
  _PCD8544.setTextSize(1);
  _PCD8544.println(); _PCD8544.print(F("V")); _PCD8544.print(REVISION_NR);
  if (_v_hold) {
    _PCD8544.print(F("   "));
    _PCD8544.setTextColor(WHITE, BLACK);
    _PCD8544.print(F(" H "));
  }
  setBatteryIcon();
  _PCD8544.display();
}

void setPCD8544Error(String errorMessage) {
  _PCD8544.clearDisplay();
  _PCD8544.setTextWrap(PCD8544_TEXT_WRAP);
  _PCD8544.setCursor(0,12);
  _PCD8544.println(errorMessage);
  _PCD8544.display();
}

void setBatteryIcon(void) {
  int
    w = 16,
    h = 8,
    x = LCDWIDTH - w,
    y = LCDHEIGHT - h;
  switch (_batt) {
    default:
    case 0:
      if (millis() % 800 < 400) _PCD8544.drawBitmap(x, y, bat_0, w, h, 1); // The battery is almost empty, the battery sign flashes each seconds
      break;
    case 1:
      _PCD8544.drawBitmap(x, y, bat_1, w, h, 1);
      break;
    case 2:
      _PCD8544.drawBitmap(x, y, bat_2, w, h, 1);
      break;
    case 3:
      _PCD8544.drawBitmap(x, y, bat_3, w, h, 1);
      break;
    case 4:
      _PCD8544.drawBitmap(x, y, bat_4, w, h, 1);
      break;
  }
}

void setLaserStatus(void) {
  digitalWrite(LASER_PIN, _v_hold ? LOW : HIGH);
}

