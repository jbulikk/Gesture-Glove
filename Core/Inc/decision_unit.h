#ifndef DECISION_UNIT_H
#define DECISION_UNIT_H

#endif /* DECISION_UNIT_H */

#include "imu.h"
#include "flex.h"
#include <stdbool.h>
#include <string.h> 

typedef void (*GestureAction)(void);

typedef struct {
    float min_value;
    float max_value;
} ThresholdAngle;

typedef enum {
    STRAIGHT,
    BENT,
    ANY
} FingerPosition;

typedef enum {
    CLOSE,
    APART,
    EACH
} FingerTouch;


typedef struct {
    FingerPosition state;
    float mid_value;
} FingerConfig;

typedef struct {
    FingerPosition thumb;
    FingerPosition index;
    FingerPosition middle;
    FingerPosition ring;
    FingerPosition pinky;
    FingerTouch e_index;
    FingerTouch e_middle;
    ThresholdAngle roll_high;
    ThresholdAngle roll_low;
    // ThresholdAngle pitch;
    // ThresholdAngle yaw;
    // GestureAction action;
    const char *message;
} GestureConfig;

typedef struct {
    GestureConfig static_part;
    uint32_t duration;
} DynamicGestureConfig;

bool check_threshold_float(float value, ThresholdAngle threshold);
bool check_threshold_uint16(uint16_t value, FingerPosition fingerState, uint16_t mid_value);
bool is_gesture_recognized(GestureConfig *gesture_arg ,ImuData *imu_arg, FlexHand *hand_arg, FlexHand *hand_mid_arg);
void recognise_gesture_and_send_by_CDC(ImuData *imu_arg, FlexHand *hand_arg, FlexHand *hand_mid_arg);
bool check_fingers_connected(uint8_t value, FingerTouch fingerTouch);