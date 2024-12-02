#ifndef FLEX_H_
#define FLEX_H_

#endif /* FLEX_H_ */

#include <stdint.h>
#include "i2c.h"

#define ORDER_THUMB  0
#define ORDER_INDEX  1
#define ORDER_MIDDLE 2
#define ORDER_RING   3
#define ORDER_PINKY  4

#define MAX_RAW_VALUE_LONG_FLEX
#define MIN_RAW_VALUE_LONG_FLEX

#define MAX_RAW_VALUE_SHORT_FLEX
#define MIN_RAW_VALUE_SHORT_FLEX

typedef struct {
    volatile uint16_t thumb;  
    volatile uint16_t index;
    volatile uint16_t middle;
    volatile uint16_t ring;  
    volatile uint16_t pinky;
} FlexHandRaw;

void flex_assign_raw_values_to_fingers(uint16_t *data_buffer, FlexHandRaw *handStructure);