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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>/*----------------------------------------------------------------gps*/
#include <stdio.h>/*-----------------------------------------------------------------gps*/


#include "../../ECUAL/KEYPAD/KEYPAD.h"


#define KEY_1       0
#define KEY_2       1
#define KEY_3       2
#define KEY_F1      3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_F2      7
#define KEY_7       8
#define KEY_8       9
#define KEY_9       10
#define KEY_F3      11
#define KEY_A       12
#define KEY_0       13
#define KEY_H       14
#define KEY_F4      15


uint8_t gu8_KeyStatesArr[KEYS] = {0};
uint8_t SysTicks = 0;
char Pressed_key = ' ';

void SysTick_CallBack(void);
void Display_Handler(void);


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
//uint8_t TxBuffer[] = "Hello World! From STM32 USB CDC Device To Virtual COM Port\r\n";
//uint8_t TxBufferLen = sizeof(TxBuffer);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
void SysTick_CallBack(void);//Keypad
void Display_Handler(void);//Keypad
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*---------------------------------------gps handle------------------------------------------*/
//uint8_t rxBuffer[128] = {0};
//uint8_t rxIndex = 0;
//uint8_t rxData;
//float nmeaLong;
//float nmeaLat;
//float utcTime;
//char northsouth;
//char eastwest;
//char posStatus;
//float decimalLong;
//float decimalLat;
//
//float nmeaToDecimal(float coordinate) {
//    int degree = (int)(coordinate/100);
//    float minutes = coordinate - degree * 100;
//    float decimalDegree = minutes / 60;
//    float decimal = degree + decimalDegree;
//    return decimal;
//}
//
//void gpsParse(char *strParse){
//  if(!strncmp(strParse, "$GPGGA", 6)){
//    sscanf(strParse, "$GPGGA,%f,%f,%c,%f,%c",
//      &utcTime, &nmeaLat, &northsouth, &nmeaLong, &eastwest);
//    decimalLat = nmeaToDecimal(nmeaLat);
//    decimalLong = nmeaToDecimal(nmeaLong);
//  }
//  else if (!strncmp(strParse, "$GPGLL", 6)){
//    sscanf(strParse, "$GPGLL,%f,%c,%f,%c,%f",
//      &nmeaLat, &northsouth, &nmeaLong, &eastwest, &utcTime);
//    decimalLat = nmeaToDecimal(nmeaLat);
//    decimalLong = nmeaToDecimal(nmeaLong);
//  }
//  else if (!strncmp(strParse, "$GPRMC", 6)){
//    sscanf(strParse, "$GPRMC,%f,%c,%f,%c,%f,%c",
//      &utcTime, &posStatus, &nmeaLat, &northsouth, &nmeaLong, &eastwest);
//    decimalLat = nmeaToDecimal(nmeaLat);
//    decimalLong = nmeaToDecimal(nmeaLong);
//  }
//}
//
//int gpsValidate(char *nmea){
//    char check[3];
//    char calculatedString[3];
//    int index;
//    int calculatedCheck;
//
//    index=0;
//    calculatedCheck=0;
//
//    // Ensure that the string starts with a "$"
//    if(nmea[index] == '$')
//        index++;
//    else
//        return 0;
//
//    //No NULL reached, 75 char largest possible NMEA message, no '*' reached
//    while((nmea[index] != 0) && (nmea[index] != '*') && (index < 75)){
//        calculatedCheck ^= nmea[index];// calculate the checksum
//        index++;
//    }
//
//    if(index >= 75){
//        return 0;// the string is too long so return an error
//    }
//
//    if (nmea[index] == '*'){
//        check[0] = nmea[index+1];    //put hex chars in check string
//        check[1] = nmea[index+2];
//        check[2] = 0;
//    }
//    else
//        return 0;// no checksum separator found therefore invalid data
//
//    sprintf(calculatedString,"%02X",calculatedCheck);
//    return((calculatedString[0] == check[0])
//        && (calculatedString[1] == check[1])) ? 1 : 0 ;
//}
//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  if(huart->Instance==USART1)
//  {
//    // if the character received is other than 'enter' ascii13, save the data in buffer
//    if(rxData!='\n' && rxIndex < sizeof(rxBuffer))
//    {
//      rxBuffer[rxIndex++]=rxData;
//    }
//    else
//    {
//      if(gpsValidate((char*) rxBuffer)) gpsParse((char*) rxBuffer);
//      rxIndex=0;
//      memset(rxBuffer,0,sizeof(rxBuffer));
//    }
//    HAL_UART_Receive_IT(&huart1,&rxData,1); // Enabling interrupt receive again
//  }
//}
/*--------------------------------------------end gps handle--------------------------------------*/

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  ////////////////////////////////////////////////////////////////////HAL_UART_Receive_IT(&huart1,&rxData,1);
  KEYPAD_Init(0, gu8_KeyStatesArr);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	    char TxBuffer_gps[16];  // Buffer for transmission
	    int TxBufferLen_gps;

	    // Convert floats to a formatted string (e.g., "12.34,56.78\n")
	    TxBufferLen_gps = snprintf(TxBuffer_gps, sizeof(TxBuffer_gps), "%c,%.5d\n", Pressed_key /*decimalLong*/, SysTicks /*decimalLat*/);

	    // Transmit via USB CDC
	    CDC_Transmit_FS((uint8_t*)TxBuffer_gps, TxBufferLen_gps);

	    //SysTick_CallBack();

	    Display_Handler();
	    HAL_Delay(100);
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Col_0_Pin|Col_1_Pin|Col_2_Pin|Col_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Row_0_Pin Row_1_Pin Row_2_Pin Row_3_Pin */
  GPIO_InitStruct.Pin = Row_0_Pin|Row_1_Pin|Row_2_Pin|Row_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Col_0_Pin Col_1_Pin Col_2_Pin Col_3_Pin */
  GPIO_InitStruct.Pin = Col_0_Pin|Col_1_Pin|Col_2_Pin|Col_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void SysTick_CallBack(void)
{
    SysTicks++;
    //Pressed_key++;
    if(SysTicks == 5) // Each 5msec
    {
    KEYPAD_Scan(0);
    SysTicks = 0;
    }
}

void Display_Handler(void)
{
	//char pressed[2];

    if(gu8_KeyStatesArr[KEY_1] == KEY_PRESSED)
    {
    //LCD_Write_Char('1');
    	Pressed_key= '1';
    while(gu8_KeyStatesArr[KEY_1] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_2] == KEY_PRESSED)
    {
    //LCD_Write_Char('2');
    	Pressed_key = '2';
    while(gu8_KeyStatesArr[KEY_2] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_3] == KEY_PRESSED)
    {
    //LCD_Write_Char('3');
    	Pressed_key = '3';
    while(gu8_KeyStatesArr[KEY_3] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_4] == KEY_PRESSED)
    {
    //LCD_Write_Char('4');
    	Pressed_key = '4';
    while(gu8_KeyStatesArr[KEY_4] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_5] == KEY_PRESSED)
    {
    //LCD_Write_Char('5');
    	Pressed_key = '5';
    while(gu8_KeyStatesArr[KEY_5] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_6] == KEY_PRESSED)
    {
    //LCD_Write_Char('6');
    	Pressed_key = '6';

    while(gu8_KeyStatesArr[KEY_6] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_7] == KEY_PRESSED)
    {
    //LCD_Write_Char('7');
    	Pressed_key = '7';

    while(gu8_KeyStatesArr[KEY_7] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_8] == KEY_PRESSED)
    {
    //LCD_Write_Char('8');
    	Pressed_key = '8';

    while(gu8_KeyStatesArr[KEY_8] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_9] == KEY_PRESSED)
    {
    //LCD_Write_Char('9');
    	Pressed_key = '9';

    while(gu8_KeyStatesArr[KEY_9] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_0] == KEY_PRESSED)
    {
    //LCD_Write_Char('0');
    	Pressed_key = '0';

    while(gu8_KeyStatesArr[KEY_0] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_A] == KEY_PRESSED)
    {
    //LCD_Write_Char('*');
    	Pressed_key = '*';

    while(gu8_KeyStatesArr[KEY_A] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_H] == KEY_PRESSED)
    {
    //LCD_Write_Char('#');
    	Pressed_key = '#';

    while(gu8_KeyStatesArr[KEY_H] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_F1] == KEY_PRESSED)
    {
    //LCD_SR();
    	Pressed_key = 'A';
    while(gu8_KeyStatesArr[KEY_F1] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_F2] == KEY_PRESSED)
    {
    //LCD_SL();
    	Pressed_key = 'B';
    while(gu8_KeyStatesArr[KEY_F2] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_F3] == KEY_PRESSED)
    {
    //LCD_Clear();
    	Pressed_key = 'C';

    while(gu8_KeyStatesArr[KEY_F3] == KEY_PRESSED);
    }
    if(gu8_KeyStatesArr[KEY_F4] == KEY_PRESSED)
    {
    //LCD_Clear();
    	Pressed_key = 'D';

    while(gu8_KeyStatesArr[KEY_F4] == KEY_PRESSED);
    }
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
