#include "decision_unit.h"

char msg3[128];

GestureConfig gestures[] = {
//     THUMB      INDEX          MIDDLE       RING       PINKY      ROLL       PITCH       YAW      ACTION       MESSAGE 
    {{2700,4000}, {0,2000}, {0,2000}, {0,2000}, {0,2000}, {75, 110},/*{,}, {,}, {,}, */ "OK"},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ "ONE"},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ "TWO"},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ "THREE"},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ "FOUR"},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ "FIVE"},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ ""},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ ""},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ ""},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ ""},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ ""},
    // {{,}, {,}, {,}, {,}, {,}, {,},/*{,}, {,}, {,}, */ ""},
};

bool check_threshold_float(float value, ThresholdAngle threshold)
{
    return value >= threshold.min_value && value <= threshold.max_value;
}

bool check_threshold_uint16(uint16_t value, ThresholdFinger threshold)
{
    return value >= threshold.min_value && value <= threshold.max_value;
}

bool is_gesture_recognized(const GestureConfig *gesture, const ImuData *imu, const FlexHandRaw *hand)
{
    return check_threshold_uint16(hand->thumb, gesture->thumb) &&
           check_threshold_uint16(hand->index, gesture->index) &&
           check_threshold_uint16(hand->middle, gesture->middle) &&
           check_threshold_uint16(hand->ring, gesture->ring) &&
           check_threshold_uint16(hand->pinky, gesture->pinky) &&
           /*check_threshold_float(imu->pitch_complementary, gesture->pitch) &&*/ 
           check_threshold_float(imu->roll_complementary, gesture->roll);
}

void recognise_gesture_and_send_by_CDC(const ImuData *imu, const FlexHandRaw *hand) {
    for (size_t i = 0; i < sizeof(gestures) / sizeof(gestures[0]); i++) {
        if (is_gesture_recognized(&gestures[i], imu, hand)) {
            sprintf(msg3, "%s\n\r", gestures[i].message);
            CDC_Transmit_FS((uint8_t *)msg3, strlen(msg3));
            break;
        }
    }
}

