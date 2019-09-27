#include "EDS.h"
#include "Hardware.h"


#define _WIRE_GPIO(WGPIO, ...)          GPIO##WGPIO
#define _WIRE_PIN1(WGPIO, WPIN1, ...)   GPIO_Pin_##WPIN1
#define _WIRE_PIN2(WGPIO, WPIN1, WPIN2) GPIO_Pin_##WPIN2
#define _WIRE_SRC1(WGPIO, WSRC1, ...)   WSRC1
#define _WIRE_SRC2(WGPIO, WSRC1, WSRC2) WSRC2
#define _WIRE1(WGPIO, WPIN1, ...)       GPIO##WGPIO, GPIO_Pin_##WPIN1
#define _WIRE2(WGPIO, WPIN1, WPIN2)     GPIO##WGPIO, GPIO_Pin_##WPIN1 | GPIO_Pin_##WPIN2
#define WIRE_GPIO(WNAME)                _WIRE_GPIO(WNAME)
#define WIRE_PIN1(WNAME)                _WIRE_PIN1(WNAME)
#define WIRE_PIN2(WNAME)                _WIRE_PIN2(WNAME)
#define WIRE_SRC1(WNAME)                _WIRE_SRC1(WNAME)
#define WIRE_SRC2(WNAME)                _WIRE_SRC2(WNAME)
#define WIRE1(WNAME)                    _WIRE1(WNAME)
#define WIRE2(WNAME)                    _WIRE2(WNAME)

#define HW_WIRE_LED1                    G, 13
#define HW_WIRE_LED2                    G, 14
#define HW_WIRE_LEDS                    G, WIRE_SRC1(HW_WIRE_LED1), WIRE_SRC1(HW_WIRE_LED2)

#define HW_LED_ON(LED)                  GPIO_SetBits(_WIRE1(LED))
#define HW_LED_OFF(LED)                 GPIO_ResetBits(_WIRE1(LED))

#define HW_ISR_PERIODIC_TIMER           TIM2_IRQHandler


static void HW_RCCInit(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

static void HW_LEDInit(void)
{
  GPIO_InitTypeDef GPIO_Configure;
  
  GPIO_Configure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_Configure.GPIO_OType = GPIO_OType_PP;
  GPIO_Configure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Configure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Configure.GPIO_Pin   = WIRE_PIN1(HW_WIRE_LEDS) | WIRE_PIN2(HW_WIRE_LEDS);
  GPIO_Init(WIRE_GPIO(HW_WIRE_LEDS), &GPIO_Configure);
  
  HW_LED_OFF(HW_WIRE_LED1);
  HW_LED_OFF(HW_WIRE_LED2);
}

static void HW_PeriodicTimerInit(void)
{
  TIM_TimeBaseInitTypeDef TIM_Configure;
  
  TIM_Configure.TIM_ClockDivision     = TIM_CKD_DIV1;
  TIM_Configure.TIM_CounterMode       = TIM_CounterMode_Up;
  TIM_Configure.TIM_Period            = 2     - 1;
  TIM_Configure.TIM_Prescaler         = 45000 - 1;
  TIM_Configure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_Configure);
  
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM2_IRQn);
}

void HW_MCUInit(void)
{
  HW_RCCInit();
  HW_LEDInit();
  HW_PeriodicTimerInit();
}

void HW_LEDOn(u8 LED)
{
  if(LED == HW_LED1)
    HW_LED_ON(HW_WIRE_LED1);
  else if(LED == HW_LED2)
    HW_LED_ON(HW_WIRE_LED2);
}

void HW_LEDOff(u8 LED)
{
  if(LED == HW_LED1)
    HW_LED_OFF(HW_WIRE_LED1);
  else if(LED == HW_LED2)
    HW_LED_OFF(HW_WIRE_LED2);
}

void HW_PeriodicTimerStart(void)
{
  TIM_Cmd(TIM2, ENABLE);
}

void HW_ISR_PERIODIC_TIMER(void)
{
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    //EDS_PutEvent(EDS_EVENT_TIMEOUT_LED1);
    
    extern u32 EDS_TaskTimeout[EDS_MAX_TASK_QUANTITY];
    
    for(u32 TaskID = 0; TaskID < EDS_MAX_TASK_QUANTITY; ++TaskID)
    {
      if(EDS_TaskTimeout[TaskID] != 0)
      {
        if(--EDS_TaskTimeout[TaskID] == 0)
          EDS_PutEvent(TaskID + 1);
      }
    }
  }
}
