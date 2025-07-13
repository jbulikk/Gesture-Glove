#include "decision_unit.h"

char msg3[128];
const char* last_gesture;

GestureConfig gestures[] = {
//     THUMB         INDEX          MIDDLE           RING            PINKY       E_I        E_M         ROLL_HIGH     ROLL_LOW    PITCH_HIGH    PITCH_LOW  MESSAGE 
    {{STRAIGHT},    {BENT},         {BENT},         {BENT},         {BENT},     {EACH},     {EACH},    {-90,-25},    {-90,-25},    {-90,60},    {-90,60},  "OK"},
    {{BENT},        {BENT},         {BENT},         {BENT},         {BENT},     {EACH},     {EACH},    {130, 190},   {-160,0},     {150,190}, {-190,-150}, "FIST"},
    {{STRAIGHT},    {STRAIGHT},     {STRAIGHT},     {STRAIGHT},     {STRAIGHT}, {EACH},     {EACH},    {130, 190},   {-160,0},     {150,190}, {-190,-150}, "IDLE"},
    {{STRAIGHT},    {BENT},         {BENT},         {BENT},         {BENT},     {EACH},     {EACH},    {130, 190},   {-160,0},     {150,190}, {-190,-150}, "ONE"},
    {{STRAIGHT},    {STRAIGHT},     {BENT},         {BENT},         {BENT},     {EACH},     {EACH},    {130, 190},   {-160,0},     {150,190}, {-190,-150}, "TWO"},
    {{STRAIGHT},    {STRAIGHT},     {STRAIGHT},     {BENT},         {BENT},     {EACH},     {EACH},    {130, 190},   {-160,0},     {150,190}, {-190,-150}, "THREE"}, 
    {{BENT},        {BENT},         {STRAIGHT},     {STRAIGHT},     {STRAIGHT}, {CLOSE},   {APART},    {-90,-25},    {-90,-25},    {-90,60},    {-90,60},  "ZERO"},
    {{BENT},        {STRAIGHT},     {STRAIGHT},     {BENT},         {BENT},     {EACH},     {EACH},    {-150, 150},  {-150, 150},  {-120,-90}, {-120,-90}, "VICTORY"},
    {{STRAIGHT},    {STRAIGHT},     {STRAIGHT},     {STRAIGHT},     {STRAIGHT}, {EACH},     {EACH},    {-150, 150},  {-150, 150},  {-120,-90}, {-120,-90}, "HI"},
};

bool check_threshold_float(float value, ThresholdAngle threshold)
{
    return value >= threshold.min_value && value <= threshold.max_value;
}

bool check_threshold_uint16(uint16_t value, FingerPosition fingerState, uint16_t mid_value)
{
    switch (fingerState) {
        case STRAIGHT:
            return value >= mid_value;
        case BENT:
            return value < mid_value;
        case ANY:
            return true;
        default:
            return false;
    }
}

bool check_fingers_connected(uint8_t value, FingerTouch fingerTouch)
{
    switch (fingerTouch) {
        case CLOSE:
            return value == 1;
        case APART:
            return value == 0;
        case EACH:
            return true;
        default:
            return false;
    }
}

bool is_gesture_recognized(GestureConfig *gesture_arg, ImuData *imu_arg, FlexHand *hand_arg, FlexHand *hand_mid_arg)
{
     return check_threshold_uint16(hand_arg->thumb, gesture_arg->thumb, hand_mid_arg->thumb) &&
           check_threshold_uint16(hand_arg->index, gesture_arg->index, hand_mid_arg->index) &&
           check_threshold_uint16(hand_arg->middle, gesture_arg->middle, hand_mid_arg->middle) &&
           check_threshold_uint16(hand_arg->ring, gesture_arg->ring, hand_mid_arg->ring) &&
           check_threshold_uint16(hand_arg->pinky, gesture_arg->pinky, hand_mid_arg->pinky) &&
           
           check_fingers_connected(hand_arg->electrodes.index, gesture_arg->e_index) &&
           check_fingers_connected(hand_arg->electrodes.middle, gesture_arg->e_middle) &&
           
           (check_threshold_float(imu_arg->roll_complementary, gesture_arg->roll_high) ||
            check_threshold_float(imu_arg->roll_complementary, gesture_arg->roll_low)) &&
            
            (check_threshold_float(imu_arg->pitch_complementary, gesture_arg->pitch_high) ||
            check_threshold_float(imu_arg->pitch_complementary, gesture_arg->pitch_low));
}

void recognise_gesture_and_send_by_CDC(ImuData *imu_arg, FlexHand *hand_arg, FlexHand *hand_mid_arg) {
    for (size_t i = 0; i < sizeof(gestures) / sizeof(gestures[0]); i++) {
        if (is_gesture_recognized(&gestures[i], imu_arg, hand_arg, hand_mid_arg)) {
            if(last_gesture == gestures[i].message) break;
            sprintf(msg3, "%s\n\r", gestures[i].message);
            CDC_Transmit_FS((uint8_t *)msg3, strlen(msg3));
            last_gesture = gestures[i].message;
            break;
        }
    }
}

