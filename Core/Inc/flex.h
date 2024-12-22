#ifndef FLEX_H_
#define FLEX_H_

#endif /* FLEX_H_ */

#include <stdint.h>
#include "i2c.h"
#include "electrodes.h"

typedef enum {
    ORDER_THUMB = 4,
    ORDER_INDEX = 1,
    ORDER_MIDDLE = 0,
    ORDER_RING = 3,
    ORDER_PINKY = 2
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
} FlexHand;

void flex_assign_raw_values_to_fingers(uint16_t *data_buffer, FlexHand *handStructure);
void calibrate_ADC_raw(uint16_t *adc_value, uint16_t (*adc_buffer)[10], uint16_t *calibration_values, int num_samples);
void calculate_ADC_raw_diff(uint16_t *adc_value_max, uint16_t *adc_value_min, uint16_t *adc_value_diff);
void calibrate_ADC_voltage(
    uint16_t *adc_value, 
    uint16_t (*adc_buffer)[10], 
    uint16_t *calibration_values, 
    float *voltage, 
    uint16_t ADC_RESOLUTION,
    int num_samples,
    float V_REF);

void calculate_ADC_voltage_diff(float *voltage_max, float *voltage_min, float *diff);
void assign_average_values(uint16_t *adc_value_max, uint16_t *diff, FlexHand *hand_mid);