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

void sum2Sensors(S *a, S b) {
  for(int i=0; i<3; i++) if (b.floatAt[i]) a->floatAt[i] += b.floatAt[i];
}

void substractVectorToSensor(S *a, V b) {
  for(int i=0; i<3; i++) if (b.floatAt[i]) a->floatAt[i] -= b.floatAt[i];
}

void divideSensor(S *a, int scalar) {
  for(int i=0; i<3; i++) if (a->floatAt[i]) a->floatAt[i] /= scalar;
}

void multiplySensorWithVector(S *a, V b) {
  for(int i=0; i<3; i++) if (a->floatAt[i]) a->floatAt[i] *= b.floatAt[i];
}

void multiplySensorWithMatrix(S *a, M m) {
  V result = {0, 0, 0};
  for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) result.floatAt[i] += m.m[i][j] * a->floatAt[j];
  for (int i=0; i<3; ++i) a->floatAt[i] = result.floatAt[i];
}

void lowPassFilter(S *a, S *y) {
  for(int i=0; i<3; i++) if (a->floatAt[i]) y->floatAt[i] = lowPassFilter(a->floatAt[i], y->floatAt[i]);
}

