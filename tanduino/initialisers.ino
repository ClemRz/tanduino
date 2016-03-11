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

void initPCD8544(void) {
  _PCD8544.begin();
  _PCD8544.setContrast(PCD8544_CONTRAST);
  #if PCD8544_FLIP
  _PCD8544.setRotation(2);
  #endif  //FLIP_DISPLAY
  _PCD8544.display();
  delay(1*SEC*MILLISEC);
  #if VERBOSE_MODE
  Serial.println(F("PCD8544 init"));
  #endif  //VERBOSE_MODE
}

void initADXL345(void) {
  if(!_ADXL345.begin()) {
    _yADXL345.failed = true;
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
    _yHMC5883.failed = true;
    return;
  }
  #if VERBOSE_MODE
  sensor_t sensor;
  mag.getSensor(&sensor);
  displaySensorDetails(sensor);
  #endif  //VERBOSE_MODE
}

void initBatt(void) {
  pinMode(BATTERY_PIN, INPUT);
  pinMode(REFERENCE_3V3_PIN, INPUT);
}

void initButton(void) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isrButton, CHANGE);
}

void initBuzzer(void) {
  pinMode(PIEZO_PIN, OUTPUT);
  longChirp();
}

void initLaser(void) {
  pinMode(LASER_PIN, OUTPUT);
}

void initSerial(void) {
  bool serial = VERBOSE_MODE || WAIT_TO_START || _calibrate;
  if (_serial != serial) {
    if (serial) {
      Serial.begin(BAUD);
    } else {
      Serial.end();
    }
    _serial = serial;
  }
}

