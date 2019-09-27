#include "EDS.h"
#include "Hardware.h"


void TaskLED1(void)
{
  while(1)
  {
    HW_LEDOn(HW_LED1);
    EDS_Timeout(50);
    HW_LEDOff(HW_LED1);
    EDS_Timeout(2100);
  }
}

void TaskLED2(void)
{
  while(1)
  {
    HW_LEDOn(HW_LED2);
    EDS_Timeout(50);
    HW_LEDOff(HW_LED2);
    EDS_Timeout(950);
  }
}

int main(void)
{
  HW_MCUInit();
  
  EDS_CreateTask(EDS_TASK_LED1, &TaskLED1, 64);
  EDS_CreateTask(EDS_TASK_LED2, &TaskLED2, 64);
  
  EDS_LaunchTask(EDS_TASK_LED1);
  EDS_LaunchTask(EDS_TASK_LED2);
  
  HW_PeriodicTimerStart();
  
  while(1)
  {
    u32 Event = EDS_GetEvent();
    if(Event)
      EDS_EventHandler(Event);
  }
}
