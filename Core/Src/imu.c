#include <math.h>
#include "imu.h"

#define BUFFER_SIZE 14
static float sampling_time_sec = 0.02;
static const uint16_t i2c_timeout = 100;
uint8_t debug = 1;
float alpha = 0.98;
uint8_t gyro_sens = 131.0;
uint8_t acc_sens = 16384.0;
float current_angle = 0.0;
char msg2[128];

uint8_t MPU6050_DMA_mode_init(I2C_HandleTypeDef *I2Cx)
{
    I2C_HandleTypeDef *i2c = I2Cx;
    uint8_t counter = 0;
    uint8_t check;
    uint8_t Data;
    
    for(int i = 0; i < 5; i++){
        HAL_Delay(1000);
        HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, 1, &check, 1, 10);

        if(check == 104)
        {
            counter++;
            if(counter >= 2)
            {
                break;
            }
        }
        sprintf(msg2, "WHO_AM_I check: %d \n\r", check);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
        HAL_Delay(1000);
    }

    Data = MPU6050_CLOCK_PLL_ZGYRO;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 1, &Data, 1, i2c_timeout);//PLL with Z axis gyroscope reference

    Data = 0x06;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_CONFIG, 1, &Data, 1, i2c_timeout);        //DLPF_CFG = 1: Fs=1khz; bandwidth=42hz 

    Data = 0x13;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_SMPLRT_DIV, 1, &Data, 1, i2c_timeout);    //50Hz sample rate

    Data = MPU6050_GYRO_FS_250;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_GYRO_CONFIG, 1, &Data, 1, i2c_timeout);    //Gyro full scale setting

    Data = MPU6050_ACCEL_FS_2;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 1, &Data, 1, i2c_timeout);    //Accel full scale setting

    // Data = 1<<4;
    Data = 0x30;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_INT_PIN_CFG, 1, &Data , 1, i2c_timeout);        //interrupt status bits are cleared on any read operation

    Data = 1<<0;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_INT_ENABLE, 1, &Data, 1, i2c_timeout);        //interupt occurs when data is ready. The interupt routine is in the receiver.c file.   

    Data = 0x07;
    HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, MPU6050_RA_SIGNAL_PATH_RESET, 1, &Data, 1, i2c_timeout);//reset gyro and accel sensor  
}

uint8_t MPU6050_standard_init(I2C_HandleTypeDef *I2Cx)
{
    I2C_HandleTypeDef *i2c = I2Cx;
    uint8_t check;
    uint8_t Data;

    for(int i = 0; i < 5; i++){
        HAL_Delay(1000);
        HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, 1, &check, 1, 10);

        sprintf(msg2, "WHO_AM_I check: %d \n\r", check);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
        HAL_Delay(1000);
    }
    

    if (check == 104) // 0x68 will be returned by the sensor if everything goes well
    {
        // power management register 0X6B we should write all 0's to wake the sensor up
        Data = 0;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x6B, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        // Set DATA RATE of 1KHz by writing SMPLRT_DIV register
        Data = 0x07;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x19, 1, &Data, 1, i2c_timeout)  != HAL_OK)
        {
            return 3;
        }

        // Set accelerometer configuration in ACCEL_CONFIG Register
        // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
        Data = 0x00;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x1C, 1, &Data, 1, i2c_timeout)  != HAL_OK)
        {
            return 4;
        }

        // Set Gyroscopic configuration in GYRO_CONFIG Register
        // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s
        Data = 0x00;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x1B, 1, &Data, 1, i2c_timeout)  != HAL_OK)
        {
            return 5;
        }
        return 0;
    }
    return 6;
}

uint8_t MPU6050_interrupt_mode_init(I2C_HandleTypeDef *I2Cx)
{
    I2C_HandleTypeDef *i2c = I2Cx;
    uint8_t check;
    uint8_t Data;

    for(int i = 0; i < 5; i++){
        HAL_Delay(1000);
        HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, 1, &check, 1, 10);

        sprintf(msg2, "WHO_AM_I check: %d \n\r", check);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
        HAL_Delay(1000);
    }
    

    if (check == 104)
    {
        Data = 0;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x6B, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        Data = 0b00000001;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x38, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        HAL_Delay(1000);

        Data = 0x80;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x37, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        Data = 0x01;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x1C, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        Data = 0x14;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x1F, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        Data = 0x28;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x20, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        Data = 0x15;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x69, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }
        Data = 0x01; 
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x38, 1, &Data, 1, i2c_timeout) != HAL_OK)
        {
            return 2;
        }

        Data = 0x07;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x19, 1, &Data, 1, i2c_timeout)  != HAL_OK)
        {
            return 3;
        }

        // Set accelerometer configuration in ACCEL_CONFIG Register
        // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
        Data = 0x00;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x1C, 1, &Data, 1, i2c_timeout)  != HAL_OK)
        {
            return 4;
        }

        // Set Gyroscopic configuration in GYRO_CONFIG Register
        // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s
        Data = 0x00;
        if(HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDRESS, 0x1B, 1, &Data, 1, i2c_timeout)  != HAL_OK)
        {
            return 5;
        }
        return 0;
    }
    return 6;
}

uint8_t MPU6050_read_acc_raw(I2C_HandleTypeDef *I2Cx, int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t tmp[6];

    if(HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, 1, tmp, 6, 10) != HAL_OK)
    {
        return 1;
    }

    *x = (int16_t)(tmp[0] << 8 | tmp[1]);
    *y = (int16_t)(tmp[2] << 8 | tmp[3]);
    *z = (int16_t)(tmp[4] << 8 | tmp[5]);

    // *a_x = ax_raw / 16384.0;
    // *a_y = ay_raw / 16384.0;
    // *a_z = az_raw / 16384.0;

    return 0;    
}

uint8_t MPU6050_read_gyro_raw(I2C_HandleTypeDef *I2Cx, int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t tmp[6];
	if(HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, 1, tmp, 6, 10) == HAL_OK){
        *x = (((int16_t)tmp[0]) << 8) | tmp[1];
        *y = (((int16_t)tmp[2]) << 8) | tmp[3];
        *z = (((int16_t)tmp[4]) << 8) | tmp[5];

        if(debug)
        {
            sprintf(msg2, "gx_raw_f:=%d, gy_raw_f:=%d, gz_raw_f:=%d \n\r", *x, *y, *z);
            CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
        }
                
        return 0;
    }
    return 1;	
}

void MPU6050_DMA_read_accelerometer(I2C_HandleTypeDef *I2Cx, uint8_t *data_buffer) 
{
    HAL_I2C_Mem_Read_DMA(I2Cx, MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, data_buffer, 6);
}

void MPU6050_DMA_read_all_data(I2C_HandleTypeDef *I2Cx, uint8_t *data_buffer)
{
    HAL_I2C_Mem_Read_DMA(I2Cx, MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, data_buffer, 14);
}

void MPU6050_DMA_read_data(I2C_HandleTypeDef *I2Cx, uint8_t *data_buffer)
{
    HAL_I2C_Mem_Read_DMA(I2Cx, MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, 1, data_buffer, 14);
}

uint8_t MPU6050_process_3_axis_data(uint8_t *data_buffer, ImuData *imuStruct)
{
    imuStruct->accelerometer_raw.x = (int16_t)(data_buffer[0] << 8 | data_buffer[1]);
    imuStruct->accelerometer_raw.y= (int16_t)(data_buffer[2] << 8 | data_buffer[3]);
    imuStruct->accelerometer_raw.z= (int16_t)(data_buffer[4] << 8 | data_buffer[5]);

    if(debug)
    {
        sprintf(msg2, "ax:=%d, ay:=%d az:=%d\n\r", imuStruct->accelerometer_raw.x, imuStruct->accelerometer_raw.y, imuStruct->accelerometer_raw.z);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
    }

    return 0;    
}

uint8_t MPU6050_process_6_axis_data(uint8_t *data_buffer, ImuData *imuStruct)
{
    imuStruct->accelerometer_raw.x = (int16_t)(data_buffer[0] << 8 | data_buffer[1]);
    imuStruct->accelerometer_raw.y= (int16_t)(data_buffer[2] << 8 | data_buffer[3]);
    imuStruct->accelerometer_raw.z= (int16_t)(data_buffer[4] << 8 | data_buffer[5]);

    imuStruct->gyroscope_raw.x = (int16_t)(data_buffer[8] << 8 | data_buffer[9]);
    imuStruct->gyroscope_raw.y = (int16_t)(data_buffer[10] << 8 | data_buffer[11]);
    imuStruct->gyroscope_raw.z = (int16_t)(data_buffer[12] << 8 | data_buffer[13]);

    if(debug==1)
    {
        sprintf(msg2, "ax:=%d, ay:=%d az:=%d\n\r", imuStruct->accelerometer_raw.x, imuStruct->accelerometer_raw.y, imuStruct->accelerometer_raw.z);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));

        sprintf(msg2, "gx:=%d, gy:=%d gz:=%d\n\r", imuStruct->accelerometer_raw.x, imuStruct->gyroscope_raw.y, imuStruct->gyroscope_raw.x);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
    }
        
    return 0;    
}

void MPU6050_process_6_axis_data_and_calculate_angles_old(uint8_t *data_buffer, ImuData *imuStruct)
{

    imuStruct->accelerometer_raw.x = (int16_t)(data_buffer[0] << 8 | data_buffer[1]);
    imuStruct->accelerometer_raw.y= (int16_t)(data_buffer[2] << 8 | data_buffer[3]);
    imuStruct->accelerometer_raw.z= (int16_t)(data_buffer[4] << 8 | data_buffer[5]);

    imuStruct->gyroscope_raw.x = (int16_t)(data_buffer[8] << 8 | data_buffer[9]);
    imuStruct->gyroscope_raw.y = (int16_t)(data_buffer[10] << 8 | data_buffer[11]);
    imuStruct->gyroscope_raw.z = (int16_t)(data_buffer[12] << 8 | data_buffer[13]);

    imuStruct->accelerometer_scaled.x = (float)imuStruct->accelerometer_raw.x / acc_sens;
	imuStruct->accelerometer_scaled.y = (float)imuStruct->accelerometer_raw.y / acc_sens;
	imuStruct->accelerometer_scaled.z = (float)imuStruct->accelerometer_raw.z / acc_sens;

    imuStruct->gyroscope_scaled.x = (float)imuStruct->gyroscope_raw.x / gyro_sens;
    imuStruct->gyroscope_scaled.y = (float)imuStruct->gyroscope_raw.y / gyro_sens;
    imuStruct->gyroscope_scaled.z = (float)imuStruct->gyroscope_raw.z / gyro_sens;

    imuStruct->roll_acc = (atan2(imuStruct->accelerometer_scaled.y, 
    sqrt(imuStruct->accelerometer_scaled.x*imuStruct->accelerometer_scaled.x + imuStruct->accelerometer_scaled.z*imuStruct->accelerometer_scaled.z))*180.0)/M_PI;
    imuStruct->roll_gyro = imuStruct->gyroscope_raw.x * 180.0 / M_PI;
    imuStruct->roll_complementary = alpha * (imuStruct->roll_complementary + imuStruct->roll_gyro * sampling_time_sec) + (1.0 - alpha) * imuStruct->roll_acc;
    
    imuStruct->pitch_acc = (atan2(imuStruct->accelerometer_scaled.x, 
    sqrt(imuStruct->accelerometer_scaled.y*imuStruct->accelerometer_scaled.y + imuStruct->accelerometer_scaled.z*imuStruct->accelerometer_scaled.z))*180.0)/M_PI;
    imuStruct->pitch_gyro = imuStruct->gyroscope_raw.y * 180.0 / M_PI;
    imuStruct->pitch_complementary = alpha * (imuStruct->pitch_complementary + imuStruct->pitch_gyro * sampling_time_sec) + (1.0 - alpha) * imuStruct->pitch_acc;
 

    if(debug == 2)
    {
        sprintf(msg2, "0:=%d, 1:=%d 2:=%d, 3:=%d, 4:=%d 5:=%d\n\r", data_buffer[0], data_buffer[1],data_buffer[2],data_buffer[3],data_buffer[4],data_buffer[5]);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
    }

    // imuStruct->accel_angle = -atan2(imuStruct->accelerometer.z, -imuStruct->accelerometer.x) * (180/M_PI);
    // imuStruct->gyro_angle = imuStruct->gyroscope.y / 131.0;  
}

void MPU6050_process_6_axis_data_and_calculate_angles(uint8_t *data_buffer, ImuData *imuStruct)
{

    imuStruct->accelerometer_raw.x = (int16_t)(data_buffer[0] << 8 | data_buffer[1]);
    imuStruct->accelerometer_raw.y= (int16_t)(data_buffer[2] << 8 | data_buffer[3]);
    imuStruct->accelerometer_raw.z= (int16_t)(data_buffer[4] << 8 | data_buffer[5]);

    imuStruct->gyroscope_raw.x = (int16_t)(data_buffer[8] << 8 | data_buffer[9]);
    imuStruct->gyroscope_raw.y = (int16_t)(data_buffer[10] << 8 | data_buffer[11]);
    imuStruct->gyroscope_raw.z = (int16_t)(data_buffer[12] << 8 | data_buffer[13]);

    imuStruct->accelerometer_scaled.x = (float)imuStruct->accelerometer_raw.x / acc_sens;
	imuStruct->accelerometer_scaled.y = (float)imuStruct->accelerometer_raw.y / acc_sens;
	imuStruct->accelerometer_scaled.z = (float)imuStruct->accelerometer_raw.z / acc_sens;

    imuStruct->gyroscope_scaled.x = (float)imuStruct->gyroscope_raw.x / gyro_sens;
    imuStruct->gyroscope_scaled.y = (float)imuStruct->gyroscope_raw.y / gyro_sens;
    imuStruct->gyroscope_scaled.z = (float)imuStruct->gyroscope_raw.z / gyro_sens;

    imuStruct->pitch_acc = atan2(imuStruct->accelerometer_scaled.y, imuStruct->accelerometer_scaled.z) * 180.0/M_PI;
    imuStruct->pitch_gyro = imuStruct->gyroscope_scaled.x;
    imuStruct->pitch_complementary = alpha * (imuStruct->pitch_complementary + imuStruct->pitch_gyro * sampling_time_sec) + (1.0 - alpha) * imuStruct->pitch_acc;
   
    imuStruct->roll_acc = atan2(imuStruct->accelerometer_scaled.x, imuStruct->accelerometer_scaled.z) * 180.0/M_PI;
    imuStruct->roll_gyro = imuStruct->gyroscope_scaled.y;
    imuStruct->roll_complementary = alpha * (imuStruct->roll_complementary + imuStruct->roll_gyro * sampling_time_sec) + (1.0 - alpha) * imuStruct->roll_acc;
   
    if(debug == 2)
    {
        sprintf(msg2, "0:=%d, 1:=%d 2:=%d, 3:=%d, 4:=%d 5:=%d\n\r", data_buffer[0], data_buffer[1],data_buffer[2],data_buffer[3],data_buffer[4],data_buffer[5]);
        CDC_Transmit_FS((uint8_t *)msg2, strlen(msg2));
    }

}