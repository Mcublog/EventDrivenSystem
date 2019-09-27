#ifndef _HARDWARE_H_
#define _HARDWARE_H_


#include "types.h"

#include <misc.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_gpio.h>


#define HW_LED1 0
#define HW_LED2 1


void HW_MCUInit(void);
void HW_LEDOn(u8 LED);
void HW_LEDOff(u8 LED);
void HW_PeriodicTimerStart(void);

#endif
