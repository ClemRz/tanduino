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

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapD(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
