void sumToVector(V *a, V b) {
  for(int i=0; i<3; i++) if (b.v[i]) a->v[i] += b.v[i];
}

void divideVector(V *a, int scalar) {
  for(int i=0; i<3; i++) if (a->v[i]) a->v[i] /= scalar;
}

void lowPassFilter(V *a, V *y) {
  for(int i=0; i<3; i++) {
    if (a->v[i]) {
      y->v[i] = getLowPassFiteredValue(a->v[i], y->v[i]);
      a->v[i] = y->v[i];
    }
  }
}

