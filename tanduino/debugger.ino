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

#if VERBOSE_MODE

void displayVectorValues(V vector) {
  #if VERBOSE_MODE > 2
  Serial.print("X: "); Serial.print(vector.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(vector.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(vector.z); Serial.print("  ");
  #endif  //VERBOSE_MODE
  #if VERBOSE_MODE > 1
  if (vector.roll) {
    Serial.print("roll: "); Serial.print(vector.roll); Serial.print("  ");
  }
  if (vector.pitch) {
    Serial.print("pitch: "); Serial.print(vector.pitch); Serial.print("  ");
  }
  if (vector.heading) {
    Serial.print("heading: "); Serial.print(vector.heading); Serial.print("  ");
  }
  Serial.println();
  #endif  //VERBOSE_MODE
}

String getSketchName(void) {
  String path = F(__FILE__);
  int slash_loc = path.lastIndexOf('/');
  String the_cpp_name = path.substring(slash_loc+1);
  int dot_loc = the_cpp_name.lastIndexOf('.');
  return the_cpp_name.substring(0, dot_loc);
}

void displaySketchInfo(void) {
  Serial.print(F("["));
  Serial.print(getSketchName());
  Serial.print(F(".ino] Rev. "));
  Serial.println(REVISION_NR);
}

void displaySensorDetails(sensor_t sensor) {
  Serial.println("------------------------------------");
  Serial.print("Sensor:       "); Serial.println(sensor.name);
  Serial.print("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
}

void displayADXL345DataRate(void) {
  Serial.print("Data Rate:    "); 
  switch (_ADXL345.getDataRate()) {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print("0.10 "); 
      break;
    default:
      Serial.print("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}

void displayADXL345Range(void) {
  Serial.print("Range:         +/- "); 
  switch (_ADXL345.getRange()) {
    case ADXL345_RANGE_16_G:
      Serial.print("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print("2 "); 
      break;
    default:
      Serial.print("?? "); 
      break;
  }  
  Serial.println(" g");  
}

#endif  //VERBOSE_MODE
