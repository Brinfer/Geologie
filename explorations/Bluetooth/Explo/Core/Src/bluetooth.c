/*
 * @file bluetooth.c
 *
 *  Created on:
 *  	@date Apr 7, 2021
 *
 *  Author:
 * 		@author Pierre-Louis GAUTIER
 */

#include "bluetooth.h"

#include "dma.h"
#include "rf.h"
#include "rtc.h"
#include "gpio.h"

extern void BLUETOOTH_new(void) {
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_RF_Init();
	MX_RTC_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Init code for STM32_WPAN */
	APPE_Init();

}
extern void BLUETOOTH_free(void) {
	// TODO
}

extern void BLUETOOTH_start(void) {
// TODO
}

extern void BLUETOOTH_stop(void) {
	// TODO
}
