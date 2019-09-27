#ifndef _EDS_H_
#define _EDS_H_


#include "types.h"


#define EDS_EVENT_BUFFER_DEPTH 16

#define EDS_EVENT_TIMEOUT_LED1 (EDS_MAX_TASK_QUANTITY + 1)
#define EDS_EVENT_TIMEOUT_LED2 (EDS_MAX_TASK_QUANTITY + 2)

#define EDS_MAX_TASK_QUANTITY  2

#define EDS_TASK_LED1          1
#define EDS_TASK_LED2          2

#define EDS_CreateTask(ID, HANDLER, STACK_SIZE) do                                                           \
                                                {                                                            \
                                                  static u32 ID##_Stack[STACK_SIZE];                         \
                                                  ID##_Stack[STACK_SIZE - 15] = 0x01000000;                  \
                                                  ID##_Stack[STACK_SIZE - 1]  = (u32)HANDLER;                \
                                                  extern void *EDS_TaskContextSP[EDS_MAX_TASK_QUANTITY + 1]; \
                                                  EDS_TaskContextSP[ID] = &ID##_Stack[STACK_SIZE - 15];      \
                                                }while(0)

#define EDS_LaunchTask(ID)                      EDS_SwitchContext(ID)


typedef struct
{
  u8 TaskID;
  u8 Semaphore;
}sEDSSemaphore;


extern sEDSSemaphore SemaphoreLED1;
extern sEDSSemaphore SemaphoreLED2;


void EDS_EventHandler(u32 Event);
u32  EDS_GetEvent(void);
void EDS_PutEvent(u32 Event);
void EDS_SemaphoreGive(sEDSSemaphore *SHandle);
void EDS_SemaphoreTake(sEDSSemaphore *SHandle);
void EDS_Timeout(u32 Timeout);

void EDS_SwitchContext(u8 TaskID);

#endif
