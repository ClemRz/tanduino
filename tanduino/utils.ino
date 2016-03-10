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

float correctedRad2Deg(float rad) {
  if(rad < 0) rad += 2 * M_PI;
  if(rad > 2 * M_PI) rad -= 2 * M_PI;
  return rad2Deg(rad);
}

float rad2Deg(float rad) {
  return rad * 180.0 / M_PI;
}

// Inspired from http://theccontinuum.com/2012/09/24/arduino-imu-pitch-roll-from-accelerometer/
float lowPassFiter(float x, float y) {
  return x * ALPHA + (y * (1.0 - ALPHA));
}

// Inspired from http://forum.arduino.cc/index.php?topic=44262.0
char *ftoa(char *a, float f, int places) {
  char *ret = a;
  long p = 1;
  for(int i=0; i<places; i++) p *= 10;
  f = round(f * p) / (float)p;
  long l = f;
  ltoa(l, a, 10);
  if (places) {
    while (*a != '\0') a++;
    *a++ = '.';
    long d = abs((f - l) * p);
    ltoa(d, a, 10);
  }
  return ret;
}

float averageAnalogRead(int pinToRead) {
  long runningValue = 0; 
  for(int x=0; x<READ_SAMPLES; x++) runningValue += analogRead(pinToRead);
  return(runningValue/READ_SAMPLES);
}

void calibarteADXL345(S *sensor) {
  multiplySensorWithVector(sensor, _orientationADXL345);
}

void calibarteHMC5883(S *sensor) {
  multiplySensorWithVector(sensor, _orientationHMC5883);
  substractVectorToSensor(sensor, _biasHMC5883);
  multiplySensorWithMatrix(sensor, _matrixHMC5883);
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapD(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void shortChirp() {
  chirp(SHORT_CHIRP);
}

void longChirp() {
  chirp(LONG_CHIRP);
}

void chirp(int duration) {
  pwmWave(PIEZO_PIN, FREQUENCY, duration);
}

void pwmWave(int pin, int frequency, int duration) {
  unsigned long t = millis();
  bool flag = true;
  int ms = round(1000000 / (2 * frequency));
  while (millis() - t < duration) {
    digitalWrite(pin, flag ? HIGH : LOW);
    flag = !flag;
    delayMicroseconds(ms);
  }
  digitalWrite(pin, LOW);
}

