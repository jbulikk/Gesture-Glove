#include "decision_unit.h"

char msg3[128];

GestureConfig gestures[] = {
//     THUMB      INDEX          MIDDLE       RING       PINKY      ROLL_HIGH       ROLL_LOW       PITCH       YAW      ACTION       MESSAGE 
    {{STRAIGHT}, {BENT}, {BENT}, {BENT}, {BENT}, {75, 110}, {75, 110},/* {,}, {,}, */ "OK"},
    {{BENT}, {BENT}, {BENT}, {BENT}, {BENT}, {-180,180}, {-180,180},/* {,}, {,}, */ "FIST"},
    {{STRAIGHT}, {STRAIGHT}, {STRAIGHT}, {STRAIGHT}, {STRAIGHT}, {-180,0},{0,180},/* {,}, {,}, */ "IDLE"},
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

bool check_threshold_uint16(uint16_t value, FingerState fingerState, FlexHand *hand_mid)
{
    uint16_t mid_value = 0;
    
    switch (fingerState) {
        case STRAIGHT:
            mid_value = hand_mid->thumb;
            return value >= mid_value;
        case BENT:
            mid_value = hand_mid->thumb;
            return value < mid_value;
        default:
            return false;
    }    
}

bool is_gesture_recognized(GestureConfig *gesture_arg, ImuData *imu_arg, FlexHand *hand_arg, FlexHand *hand_mid_arg)
{
     return check_threshold_uint16(hand_arg->thumb, gesture_arg->thumb, hand_mid_arg) &&
           check_threshold_uint16(hand_arg->index, gesture_arg->index, hand_mid_arg) &&
           check_threshold_uint16(hand_arg->middle, gesture_arg->middle, hand_mid_arg) &&
           check_threshold_uint16(hand_arg->ring, gesture_arg->ring, hand_mid_arg) &&
           check_threshold_uint16(hand_arg->pinky, gesture_arg->pinky, hand_mid_arg) &&
           
           (check_threshold_float(imu_arg->roll_complementary, gesture_arg->roll_high) ||
            check_threshold_float(imu_arg->roll_complementary, gesture_arg->roll_low));
}

void recognise_gesture_and_send_by_CDC(ImuData *imu_arg, FlexHand *hand_arg, FlexHand *hand_mid_arg) {
    for (size_t i = 0; i < sizeof(gestures) / sizeof(gestures[0]); i++) {
        if (is_gesture_recognized(&gestures[i], imu_arg, hand_arg, hand_mid_arg)) {
            sprintf(msg3, "%s\n\r", gestures[i].message);
            CDC_Transmit_FS((uint8_t *)msg3, strlen(msg3));
            break;
        }
    }
}

