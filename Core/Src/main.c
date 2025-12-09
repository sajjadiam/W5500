/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include "ALCD.h"
#include "w5500.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DMA_INDEX_NTC				0
#define DMA_MAX_SAMPLE			100.0f
#define R_FIXED							9950.0f
#define ADC_MAX							4095.0f
#define B										3950.0f
#define T0									298.15

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t dma_buff[100];
float NTC_avg = 0.0;
float NTC_tmp = 0.0;
float NTC_avg2 = 0.0;
float NTC_tmp2 = 0.0;
uint8_t lcdUpdateFlag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
double dma_avg(uint16_t* buff ,uint16_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	W5500_Handler	hw5;
	W5500_NetConfig net_cfg = {
		.mac     = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03},
    .ip      = {192, 168, 30, 79},
    .gateway = {192, 168, 30, 100},
    .subnet  = {255, 255, 255, 0},
    .src_port = 8006, // فعلاً استفاده نمی‌کنیم
	};
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t z = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	W5500_Handle_init	(&hw5);
	W5500_HardReset(&hw5);
	W5500_SoftReset(&hw5);
	uint8_t test = W5500_Version(&hw5);
	if (W5500_NetConfigure(&hw5, &net_cfg) != HAL_OK) {
    // اینجا یک LED error یا breakpoint بگذار
		z = 1;
	}
	// حالا صبر کن لینک بالا بیاد (مثلاً تا 5 ثانیه):
	if (W5500_WaitForLink(&hw5, 10000) != HAL_OK) {
    // اینجا لینک بالا نیومده -> کابل قطع یا سوییچ خاموش
    // می‌تونی LED قرمز کنی یا خطا لاگ بگیری
		y = 1;
	} 
	else {
		// اینجا لینک up هست، می‌تونیم بریم سراغ مرحله‌ی بعد (socket ها)
		x = 1;
		
	}
	
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)dma_buff,100);
	ALCD_init(16, 2);
	ALCD_clear();
	char lcd_buff[20];
	uint32_t NT = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint8_t flag = 0;
  while (1)
  {
		
		if(lcdUpdateFlag && HAL_GetTick() >  NT + 500){
			NT = HAL_GetTick();
			lcdUpdateFlag = 0;
			ALCD_goto(0, 0);
			snprintf(lcd_buff,13,"temp = %2.1fC",NTC_tmp);
			ALCD_puts(lcd_buff);
			ALCD_goto(0, 1);
			snprintf(lcd_buff,13,"temp = %2.1fC",NTC_tmp2);
			ALCD_puts(lcd_buff);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	
		uint32_t sum0 = 0;
		uint32_t sum1 = 0;
		for(uint8_t i = 0;i<DMA_MAX_SAMPLE;i+=2){
			sum0 += dma_buff[i];
		}
		for(uint8_t i = 1;i<DMA_MAX_SAMPLE;i+=2){
			sum1 += dma_buff[i];
		}
		NTC_avg = (float)sum0 / 50;
		NTC_avg2 = (float)sum1 / 50;
		float Volt = ((NTC_avg / ADC_MAX) * 3.273);
		float R_ntc = (R_FIXED * Volt) /  (3.273 - Volt);
		float invT = (1 / (float)T0) +  (logf(R_ntc / 50000.0f) / (float)B);
		float T_kelvin = 1.0f / invT;
    NTC_tmp = T_kelvin - 273.15;
		float Volt2 = ((NTC_avg2 / ADC_MAX) * 3.273);
		float R_ntc2 = (R_FIXED * Volt2) /  (3.273 - Volt2);
		float invT2 = (1 / (float)T0) +  (logf(R_ntc2 / 10000.0f) / (float)B);
		float T_kelvin2 = 1.0f / invT2;
    NTC_tmp2 = T_kelvin2 - 273.15;
		lcdUpdateFlag = 1;
}
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
#ifdef USE_FULL_ASSERT
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
