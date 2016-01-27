void sumToVector(V *a, V b) {
  for(int i=0; i<6; i++) if (b.v[i]) a->v[i] += b.v[i];
}

void divideVector(V *a, int scalar) {
  for(int i=0; i<6; i++) if (a->v[i]) a->v[i] /= scalar;
}

