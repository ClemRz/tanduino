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

void setPCD8544(void) {
  _PCD8544.clearDisplay();
  _PCD8544.setTextWrap(PCD8544_TEXT_WRAP);
  _PCD8544.setTextColor(BLACK);
  if (_calibrate) {
    setCalibrationDisplay();
  } else {
    setNormalDisplay();
  }
  if (_v_hold) {
    _PCD8544.print(F("   "));
    _PCD8544.setTextColor(WHITE, BLACK);
    _PCD8544.print(F(" H "));
  }
  if (!_calibrate) setBatteryIcon();
  _PCD8544.display();
}

void setNormalDisplay(void) {
  char buffer[7];
  _PCD8544.setTextSize(2);
  if (_yADXL345.failed) {
    _PCD8544.print(getError(buffer, ERROR_ADXL345_INIT));
  } else {
    _PCD8544.print(ftoa(buffer, rad2Deg(_yADXL345.pitch - PITCH_0), 1)); _PCD8544.write(0xF7);
  }
  _PCD8544.println();
  if (_yHMC5883.failed) {
    _PCD8544.print(getError(buffer, ERROR_HMC5883_INIT));
  } else {
    _PCD8544.print(ftoa(buffer, correctedRad2Deg(_yHMC5883.heading), 1)); _PCD8544.write(0xF7); _PCD8544.print('N');
  }
  _PCD8544.println();
  _PCD8544.setTextSize(1);
  _PCD8544.println(); _PCD8544.print(F("V")); _PCD8544.print(REVISION_NR);
}

void setCalibrationDisplay(void) {
  char buffer[7];
  _PCD8544.setCursor(15,0); _PCD8544.println(F("Acc"));
  _PCD8544.setCursor(55,0); _PCD8544.println(F("Mag"));
  _PCD8544.print(F("X ")); _PCD8544.println(_yADXL345.x);
  _PCD8544.print(F("Y ")); _PCD8544.println(_yADXL345.y);
  _PCD8544.print(F("Z ")); _PCD8544.print(_yADXL345.z);
  _PCD8544.setCursor(48,8); _PCD8544.print(_yHMC5883.x);
  _PCD8544.setCursor(48,16); _PCD8544.print(_yHMC5883.y);
  _PCD8544.setCursor(48,24); _PCD8544.print(_yHMC5883.z);
  _PCD8544.println();
  _PCD8544.println();
  _PCD8544.print(F("Batt ")); _PCD8544.print(getBatteryVoltage()); _PCD8544.print(F("V"));
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
      if (millis() % 800 < 400) _PCD8544.drawBitmap(x, y, bat_0, w, h, 1); // The battery is almost empty, the battery sign flashes
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

void setADXL345ToStandbyMode(void) {
  _ADXL345.writeRegister(ADXL345_REG_POWER_CTL, ADXL345_STANDBY);
}

void setADXL345ToMeasureMode(void) {
  _ADXL345.writeRegister(ADXL345_REG_POWER_CTL, ADXL345_MEASURE);
}

void setHMC5883ToStandbyMode(void) {
  write8(HMC5883_ADDRESS_MAG, HMC5883_REGISTER_MAG_MR_REG_M, HMC5883_IDLE);
}

void setHMC5883ToMeasureMode(void) {
  write8(HMC5883_ADDRESS_MAG, HMC5883_REGISTER_MAG_MR_REG_M, HMC5883_MEASURE);
}

