/*
 * File: KEYPAD.c
 * Driver Name: [[ 4x4 KEYPAD ]]
 * SW Layer:   ECUAL
 * Created on: Jun 28, 2020
 * Author:     Khaled Magdy
 * -------------------------------------------
 * For More Information, Tutorials, etc.
 * Visit Website: www.DeepBlueMbedded.com
 *
 */

#include "KEYPAD.h"
#include "KEYPAD_cfg.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>


typedef struct
{
	uint8_t*  KEY_States;
}KEYPAD_info;

static KEYPAD_info gs_KEYPAD_info[KEYPAD_UNITS] = {0};



void KEYPAD_Init(uint16_t au16_Instance, uint8_t* au8_KeyStates)
{
	uint8_t i = 0;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(au8_KeyStates != NULL)
	{
		gs_KEYPAD_info[au16_Instance].KEY_States = au8_KeyStates;
	}
	else
	{
		/* Potentially Should Return An Error */
		return;
	}
	/*--------[ Configure The KeyPAD ROWs GPIO Pins ]-------*/
	for(i=0; i<ROWS; i++)
	{
		if(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i] == GPIOA)
	    {
	        __HAL_RCC_GPIOA_CLK_ENABLE();
	    }
		else if(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i] == GPIOB)
		{
			__HAL_RCC_GPIOB_CLK_ENABLE();
		}
		else if(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i] == GPIOC)
		{
			__HAL_RCC_GPIOC_CLK_ENABLE();
		}
		else if(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i] == GPIOD)
		{
			__HAL_RCC_GPIOD_CLK_ENABLE();
		}
		else if(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i] == GPIOE)
		{
			__HAL_RCC_GPIOE_CLK_ENABLE();
		}
		GPIO_InitStruct.Pin = KEYPAD_CfgParam[au16_Instance].ROW_PIN[i];
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i], &GPIO_InitStruct);
	}
	/*--------[ Configure The KeyPAD ROWs GPIO Pins ]-------*/
	for(i=0; i<ROWS; i++)
	{
		if(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i] == GPIOA)
	    {
	        __HAL_RCC_GPIOA_CLK_ENABLE();
	    }
		else if(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i] == GPIOB)
		{
			__HAL_RCC_GPIOB_CLK_ENABLE();
		}
		else if(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i] == GPIOC)
		{
			__HAL_RCC_GPIOC_CLK_ENABLE();
		}
		else if(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i] == GPIOD)
		{
			__HAL_RCC_GPIOD_CLK_ENABLE();
		}
		else if(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i] == GPIOE)
		{
			__HAL_RCC_GPIOE_CLK_ENABLE();
		}
		GPIO_InitStruct.Pin = KEYPAD_CfgParam[au16_Instance].COL_PIN[i];
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		HAL_GPIO_Init(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i], &GPIO_InitStruct);
	}
}



void KEYPAD_Scan(uint16_t au16_Instance)
{
	uint8_t au8_ReadState[ROWS] = {0};
	uint8_t au8_ReadSt;
	uint8_t i=0;

	/* Steady State: Output The Pattern [1 1 1 1] To The COL Pins */
	for(i=0; i<COLS; i++)
	{
		HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i], KEYPAD_CfgParam[au16_Instance].COL_PIN[i], 1);
	}
	/* Steady State: Read The ROW Pins To Know If Any Key Is Pressed At All or Not */
	for(i=0; i<ROWS; i++)
	{
		au8_ReadState[i] = HAL_GPIO_ReadPin(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[i], KEYPAD_CfgParam[au16_Instance].ROW_PIN[i]);
	}
//	/* If Any member in array au8_ReadState is 1, we know that a key is pressed */
//	/* And we'll keep changing the COL pins output pattern until we find the pressed key or keys */
//	/* Else: We'll end the scan procedure at this point */
	if(au8_ReadState[0] | au8_ReadState[1] | au8_ReadState[2] | au8_ReadState[3])
	{
		/*--[ First Row Check ]--*/
		if(au8_ReadState[0])
		{
			/* Output To The COL Pins This Pattern: [1 0 0 0] */
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[0], KEYPAD_CfgParam[au16_Instance].COL_PIN[0], 1);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[1], KEYPAD_CfgParam[au16_Instance].COL_PIN[1], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[2], KEYPAD_CfgParam[au16_Instance].COL_PIN[2], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[3], KEYPAD_CfgParam[au16_Instance].COL_PIN[3], 0);
			/* Read ROW0-COL0 */
			au8_ReadSt = HAL_GPIO_ReadPin(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[0], KEYPAD_CfgParam[au16_Instance].ROW_PIN[0]);
			if(au8_ReadSt)
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[0] = 1;
			}
			else
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[0] = 0;
			}
			/* Output To The COL Pins This Pattern: [0 1 0 0] */
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[0], KEYPAD_CfgParam[au16_Instance].COL_PIN[0], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[1], KEYPAD_CfgParam[au16_Instance].COL_PIN[1], 1);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[2], KEYPAD_CfgParam[au16_Instance].COL_PIN[2], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[3], KEYPAD_CfgParam[au16_Instance].COL_PIN[3], 0);
			/* Read ROW0-COL1 */
			au8_ReadSt = HAL_GPIO_ReadPin(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[0], KEYPAD_CfgParam[au16_Instance].ROW_PIN[0]);
			if(au8_ReadSt)
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[1] = 1;
			}
			else
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[1] = 0;
			}
			/* Output To The COL Pins This Pattern: [0 0 1 0] */
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[0], KEYPAD_CfgParam[au16_Instance].COL_PIN[0], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[1], KEYPAD_CfgParam[au16_Instance].COL_PIN[1], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[2], KEYPAD_CfgParam[au16_Instance].COL_PIN[2], 1);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[3], KEYPAD_CfgParam[au16_Instance].COL_PIN[3], 0);
			/* Read ROW0-COL2 */
			au8_ReadSt = HAL_GPIO_ReadPin(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[0], KEYPAD_CfgParam[au16_Instance].ROW_PIN[0]);
			if(au8_ReadSt)
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[2] = 1;
			}
			else
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[2] = 0;
			}
			/* Output To The COL Pins This Pattern: [0 0 0 1] */
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[0], KEYPAD_CfgParam[au16_Instance].COL_PIN[0], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[1], KEYPAD_CfgParam[au16_Instance].COL_PIN[1], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[2], KEYPAD_CfgParam[au16_Instance].COL_PIN[2], 0);
			HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[3], KEYPAD_CfgParam[au16_Instance].COL_PIN[3], 1);
			/* Read ROW0-COL3 */
			au8_ReadSt = HAL_GPIO_ReadPin(KEYPAD_CfgParam[au16_Instance].ROW_GPIO[0], KEYPAD_CfgParam[au16_Instance].ROW_PIN[0]);
			if(au8_ReadSt)
			{
				gs_KEYPAD_info[au16_Instance].KEY_States[3] = 1;
			}
		}
		else
		{
			gs_KEYPAD_info[au16_Instance].KEY_States[0] = 0;
			gs_KEYPAD_info[au16_Instance].KEY_States[1] = 0;
			gs_KEYPAD_info[au16_Instance].KEY_States[2] = 0;
			gs_KEYPAD_info[au16_Instance].KEY_States[3] = 0;
		}

		/*---------------------[END OF 4th ROW Check ]--------------------*/
	}
	/* Else If No Key Is Pressed */
	else
	{
		/* Fill The Keys States With 0s */
		for(i=0; i<KEYS; i++)
		{
			gs_KEYPAD_info[au16_Instance].KEY_States[i] = 0;
		}
	}

	/* Back To Steady State: Output The Pattern [1 1 1 1] To The COL Pins */
	for(i=0; i<COLS; i++)
	{
		//HAL_GPIO_WritePin(KEYPAD_CfgParam[au16_Instance].COL_GPIO[i], KEYPAD_CfgParam[au16_Instance].COL_PIN[i], 1);
	}
}
