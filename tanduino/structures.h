#ifndef _TANDUINO_STRUCTURES_H
#define _TANDUINO_STRUCTURES_H

/* struct V is used to return a vector in a common format. */
typedef struct {
    union {
        float v[6];
        struct {
            float x;
            float y;
            float z;
            float roll;    // Rotation around the longitudinal axis (the plane body, 'X axis'). Roll is positive and increasing when moving downward. -90°<=roll<=90°
            float pitch;   // Rotation around the lateral axis (the wing span, 'Y axis'). Pitch is positive and increasing when moving upwards. -180°<=pitch<=180°)
            float heading; // Angle between the longitudinal axis (the plane body) and magnetic north, measured clockwise when viewing from the top of the device. 0-359°
        };
    };
} V;

#endif  //_TANDUINO_STRUCTURES_H
