#ifndef DECISION_UNIT_H
#define DECISION_UNIT_H

#endif /* DECISION_UNIT_H */

#include "imu.h"
#include "flex.h"
#include <stdbool.h>

typedef void (*GestureAction)(void);

typedef struct {
    float min_value;
    float max_value;
} ThresholdAngle;

typedef struct {
    uint16_t min_value;
    uint16_t max_value;
} ThresholdFinger;

typedef struct {
    ThresholdFinger thumb;
    ThresholdFinger index;
    ThresholdFinger middle;
    ThresholdFinger ring;
    ThresholdFinger pinky;
    ThresholdAngle roll;
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
bool check_threshold_uint16(uint16_t value, ThresholdFinger threshold);
bool is_gesture_recognized(const GestureConfig *gesture, const ImuData *imu, const FlexHandRaw *hand);
void recognise_gesture_and_send_by_CDC(const ImuData *imu, const FlexHandRaw *hand);