/*
 * File: KEYPAD_cfg.c
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
#include "main.h"

const KEYPAD_CfgType KEYPAD_CfgParam[KEYPAD_UNITS] =
{
	// KeyPAD Unit 1 Configurations
    {
    	/* ROWs Pins Info */
	    {GPIOA, GPIOA, GPIOA, GPIOA},
		{Row_0_Pin, Row_1_Pin, Row_2_Pin, Row_3_Pin},
		/* COLs Pins */
		{GPIOB, GPIOB, GPIOB, GPIOB},
		{Col_0_Pin, Col_1_Pin, Col_2_Pin, Col_3_Pin}
	}
};
