#ifndef IMU_H_
#define IMU_H_

#endif /* INC_GY521_H_ */

#include <stdint.h>
#include <math.h>
#include "i2c.h"

#include "C:\Users\JakubBulik\STM32CubeIDE\Glove\MPU6050_STM32_HAL\Inc\mpu6050.h"

typedef struct {
    volatile int16_t x;  
    volatile int16_t y;
    volatile int16_t z;
} AxisData;

typedef struct {
    volatile AxisData accelerometer_raw;
    volatile AxisData gyroscope_raw;
    volatile AxisData accelerometer_scaled;
    volatile AxisData gyroscope_scaled;
    volatile float pitch_acc;
    volatile float roll_acc;
    volatile float yaw_acc;
    volatile float pitch_gyro;
    volatile float roll_gyro;
    volatile float yaw_gyro;
    volatile float pitch_complementary;
    volatile float roll_complementary;
} ImuData;

uint8_t MPU6050_DMA_mode_init(I2C_HandleTypeDef *I2Cx);
uint8_t MPU6050_standard_init(I2C_HandleTypeDef *I2Cx);
uint8_t MPU6050_interrupt_mode_init(I2C_HandleTypeDef *I2Cx);

uint8_t MPU6050_read_acc_raw(I2C_HandleTypeDef *I2Cx, int16_t *x, int16_t *y, int16_t *z);
uint8_t MPU6050_read_gyro_raw(I2C_HandleTypeDef *I2Cx, int16_t *x, int16_t *y, int16_t *z);

void MPU6050_DMA_read_accelerometer(I2C_HandleTypeDef *I2Cx, uint8_t *data_buffer);
void MPU6050_DMA_read_all_data(I2C_HandleTypeDef *I2Cx, uint8_t *data_buffer);
void MPU6050_DMA_read_data(I2C_HandleTypeDef *I2Cx, uint8_t *data_buffer);

uint8_t MPU6050_process_3_axis_data(uint8_t *data_buffer, ImuData *imuStruct);
uint8_t MPU6050_process_6_axis_data(uint8_t *data_buffer, ImuData *imuStruct);
void MPU6050_process_6_axis_data_and_calculate_angles(uint8_t *data_buffer, ImuData *imuStruct);
void MPU6050_process_6_axis_data_and_calculate_angles_old(uint8_t *data_buffer, ImuData *imuStruct);