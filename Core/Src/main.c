#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
// #include "imu.h"
#include "decision_unit.h"

#define NUM_SAMPLES 10
#define ADC_RESOLUTION 4095
#define V_REF 5 

char msg[50];
volatile uint8_t MPU6050_buff[14];
volatile static uint16_t adc_value[5];
float current_angle_main;
volatile ImuData imu_sensor_data;
volatile FlexHand hand;
volatile FlexHand hand_mid;
volatile uint32_t tick;
volatile uint32_t last_debounce_time = 0;
static const uint8_t debounce_delay = 800;
volatile GPIO_PinState current_time;
volatile GPIO_PinState last_state = GPIO_PIN_SET;
uint16_t max_init_adc_values[7];
uint16_t diff_init_adc_values[7];
uint16_t min_init_adc_values[7];
uint16_t adc_buffer[7][NUM_SAMPLES];

void SystemClock_Config(void);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

int main(void)
{  
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();
  MPU6050_DMA_mode_init(&hi2c1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_value, 7);
  HAL_Delay(2000);
  calibrate_ADC_raw(adc_value, adc_buffer, max_init_adc_values, NUM_SAMPLES);
  sprintf(msg, "max: 1:=%u, 2:=%u, 3:=%u, 4:=%u, 5:=%u\n\r", max_init_adc_values[4], max_init_adc_values[1], max_init_adc_values[0], max_init_adc_values[3], max_init_adc_values[2]);
  CDC_Transmit_FS((uint8_t *)msg, strlen(msg));

  HAL_Delay(2000);
  calibrate_ADC_raw(adc_value, adc_buffer, min_init_adc_values, NUM_SAMPLES);
  
  sprintf(msg, "min: 1:=%u, 2:=%u, 3:=%u, 4:=%u, 5:=%u\n\r", min_init_adc_values[4], min_init_adc_values[1], min_init_adc_values[0], min_init_adc_values[3], min_init_adc_values[2]);
  CDC_Transmit_FS((uint8_t *)msg, strlen(msg));

  calculate_ADC_raw_diff(max_init_adc_values, min_init_adc_values, diff_init_adc_values);
  assign_average_values(max_init_adc_values, diff_init_adc_values, &hand_mid);


  while (1)
  {
    HAL_Delay(500);
    recognise_gesture_and_send_by_CDC(&imu_sensor_data, &hand, &hand_mid);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
  tick++;

  // if(tick >= 20)
  // {
    

  //   tick = 0;
  // }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  MPU6050_process_6_axis_data_and_calculate_angles(&MPU6050_buff, &imu_sensor_data);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
      flex_assign_raw_values_to_fingers(&adc_value, &hand);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_5)
  {
    MPU6050_DMA_read_all_data(&hi2c1, &MPU6050_buff);
  }

  if (GPIO_Pin == GPIO_PIN_12 || GPIO_Pin == GPIO_PIN_13) 
  {
    if (GPIO_Pin == GPIO_PIN_12) 
    {
      uint32_t current_time = HAL_GetTick();
      static uint32_t last_debounce_time = 0;
      static GPIO_PinState last_state = GPIO_PIN_SET;
      GPIO_PinState current_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);

      if ((current_time - last_debounce_time) > debounce_delay && current_state != last_state) 
      {
        last_debounce_time = current_time;
        last_state = current_state;

        if (current_state == GPIO_PIN_SET)
        {
          hand.electrodes.middle = 0;
        }
        else
        {
          hand.electrodes.middle = 1;
        }
      }
    }

    if (GPIO_Pin == GPIO_PIN_13) 
    {
      uint32_t current_time = HAL_GetTick();
      static uint32_t last_debounce_time = 0;
      static GPIO_PinState last_state = GPIO_PIN_SET;
      GPIO_PinState current_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);

      if ((current_time - last_debounce_time) > debounce_delay && current_state != last_state) 
      {
        last_debounce_time = current_time;
        last_state = current_state;

        if (current_state == GPIO_PIN_SET)
        {
          hand.electrodes.index = 0;
        }
        else
        {
          hand.electrodes.index = 1; 
        }
      }
    }
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
