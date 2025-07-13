#include "flex.h"

void flex_assign_raw_values_to_fingers(uint16_t *data_buffer, FlexHand *handStructure)
{
    handStructure->thumb = data_buffer[ORDER_THUMB];
    handStructure->index = data_buffer[ORDER_INDEX];
    handStructure->middle = data_buffer[ORDER_MIDDLE];
    handStructure->ring = data_buffer[ORDER_RING];
    handStructure->pinky = data_buffer[ORDER_PINKY];
}

void calibrate_ADC_raw(uint16_t *adc_value, uint16_t (*adc_buffer)[10], uint16_t *calibration_values, int num_samples)
{

    for (int i = 0; i < num_samples; i++) {
        HAL_Delay(10); 

        for (int j = 0; j < 7; j++) {
            adc_buffer[j][i] = adc_value[j];
        }
    }

    for (int i = 0; i < 7; i++) {
        uint16_t sum = 0;
        for (int j = 0; j < num_samples; j++) {
            sum += adc_buffer[i][j];
        }
        calibration_values[i] = sum / num_samples;
    }

}

void calculate_ADC_raw_diff(uint16_t *adc_value_max, uint16_t *adc_value_min, uint16_t *adc_value_diff)
{
    for(int i = 0; i < 7; i++)
    {
        adc_value_diff[i] = adc_value_max[i] - adc_value_min[i];
    }
}

void assign_average_values(uint16_t *adc_value_max, uint16_t *diff, FlexHand *hand_mid)
{
    hand_mid->thumb = adc_value_max[ORDER_THUMB]  - diff[ORDER_THUMB]/2;
    hand_mid->index = adc_value_max[ORDER_INDEX]  - diff[ORDER_INDEX]/2;
    hand_mid->middle = adc_value_max[ORDER_MIDDLE]- diff[ORDER_MIDDLE]/2;
    hand_mid->ring = adc_value_max[ORDER_RING]    - diff[ORDER_RING]/2;
    hand_mid->pinky = adc_value_max[ORDER_PINKY]  - diff[ORDER_PINKY]/2;
}

void calibrate_ADC_voltage(uint16_t *adc_value, uint16_t (*adc_buffer)[10], uint16_t *calibration_values, float *voltage, 
                    uint16_t ADC_RESOLUTION, int num_samples, float V_REF) 
{

    for (int i = 0; i < num_samples; i++) {
        HAL_Delay(10); 

        for (int j = 0; j < 7; j++) {
            adc_buffer[j][i] = adc_value[j];
        }
    }

    for (int i = 0; i < 7; i++) {
        uint16_t sum = 0;
        for (int j = 0; j < num_samples; j++) {
            sum += adc_buffer[i][j];
        }
        calibration_values[i] = sum / num_samples;
    }

    for (int i = 0; i < 7; i++) {
        voltage[i] = ((float)adc_value[i] / ADC_RESOLUTION) * V_REF;
    }
}

void calculate_ADC_voltage_diff(float *voltage_max, float *voltage_min, float *diff)
{
    for(int i = 0; i < 7; i++)
    {
        diff[i] = voltage_max[i] - voltage_min[i];
    }
}

