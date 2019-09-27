#include "EDS.h"


#define SEMAPHORE_RESET 0
#define SEMAPHORE_SET   1
#define SEMAPHORE_WAIT  2


u8            EDS_CurrentTaskID;
u32           EDS_TaskTimeout[EDS_MAX_TASK_QUANTITY];
void         *EDS_TaskContextSP[EDS_MAX_TASK_QUANTITY + 1];


static u32    EventBuffer[EDS_EVENT_BUFFER_DEPTH];
static u32    EventCounter;
static u32    ReadPosition;
static u32    WritePosition;

sEDSSemaphore SemaphoreLED1;
sEDSSemaphore SemaphoreLED2;


static void Callback_TimeoutLED1(void)
{
  EDS_SemaphoreGive(&SemaphoreLED1);
}

static void Callback_TimeoutLED2(void)
{
  EDS_SemaphoreGive(&SemaphoreLED2);
}

void EDS_EventHandler(u32 Event)
{
  static void (*HandlerArray[])(void) =
  {
    &Callback_TimeoutLED1,
    &Callback_TimeoutLED2
  };
  
  if(Event <= EDS_MAX_TASK_QUANTITY)
    EDS_SwitchContext(Event);
  else
    HandlerArray[Event - EDS_MAX_TASK_QUANTITY - 1]();
}

u32 EDS_GetEvent(void)
{
  u32 Event;
  
  if(EventCounter > 0)
  {
    __disable_irq();
    
    --EventCounter;
    
    Event = EventBuffer[ReadPosition++];
    if(ReadPosition == EDS_EVENT_BUFFER_DEPTH)
      ReadPosition = 0;
    
    __enable_irq();
    
    return Event;
  }
  else
    return 0;
}

void EDS_PutEvent(u32 Event)
{
  __disable_irq();
  
  if(EventCounter < EDS_EVENT_BUFFER_DEPTH)
  {
     EventBuffer[WritePosition++] = Event;
     if(WritePosition == EDS_EVENT_BUFFER_DEPTH)
       WritePosition = 0;
     
     EventCounter++;
  }
  
  __enable_irq();
}

void EDS_SemaphoreGive(sEDSSemaphore *SHandle)
{
  u8 SemaphoreState  = SHandle->Semaphore;
  SHandle->Semaphore = SEMAPHORE_SET;
  
  if(SemaphoreState == SEMAPHORE_WAIT)
    EDS_SwitchContext(SHandle->TaskID);
}

void EDS_SemaphoreTake(sEDSSemaphore *SHandle)
{
  if(SHandle->Semaphore == SEMAPHORE_SET)
    SHandle->Semaphore = SEMAPHORE_RESET;
  else
  {
    SHandle->TaskID    = EDS_CurrentTaskID;
    SHandle->Semaphore = SEMAPHORE_WAIT;
    
    EDS_SwitchContext(0);
    
    SHandle->Semaphore = SEMAPHORE_RESET;
  }
}

void EDS_Timeout(u32 Timeout)
{
  if(Timeout != 0)
  {
    __disable_irq();
    
    EDS_TaskTimeout[EDS_CurrentTaskID - 1] = Timeout;
    
    __enable_irq();
    
    EDS_SwitchContext(0);
  }
}
