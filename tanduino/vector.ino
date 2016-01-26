void sumToVector(sensors_vec_t *a, sensors_vec_t b) {
  if (b.x) a->x += b.x;
  if (b.y) a->y += b.y;
  if (b.z) a->z += b.z;
  if (b.pitch) a->pitch += b.pitch;
  if (b.roll) a->roll += b.roll;
  if (b.heading) a->heading += b.heading;
}

sensors_vec_t divideVector(sensors_vec_t *a, int scalar) {
  if (a->x) a->x /= scalar;
  if (a->y) a->y /= scalar;
  if (a->z) a->z /= scalar;
  if (a->pitch) a->pitch /= scalar;
  if (a->roll) a->roll /= scalar;
  if (a->heading) a->heading /= scalar;
}

