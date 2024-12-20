#ifndef FLEX_H_
#define FLEX_H_

#endif /* FLEX_H_ */

#include <stdint.h>
#include "i2c.h"
#include "electrodes.h"

typedef enum {
    ORDER_THUMB = 4,
    ORDER_INDEX = 1,
    ORDER_MIDDLE = 2,
    ORDER_RING = 3,
    ORDER_PINKY = 0
} FingersOrder;

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
    volatile Electrodes electrodes;
} FlexHandRaw;

void flex_assign_raw_values_to_fingers(uint16_t *data_buffer, FlexHandRaw *handStructure);