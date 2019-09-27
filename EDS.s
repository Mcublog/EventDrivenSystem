    PRESERVE8
    THUMB
    AREA |.text|, CODE, READONLY
    
EDS_SwitchContext PROC
                  EXPORT EDS_SwitchContext
                  IMPORT EDS_CurrentTaskID
                  IMPORT EDS_TaskContextSP
                  
                  push {lr, r0-r12}
                  
                  mrs  r1, PSR
                  push {r1}
                  
                  ldr  r1, =EDS_TaskContextSP
                  ldr  r2, =EDS_CurrentTaskID
                  ldrb r3, [r2]
                  
                  str  sp, [r1, r3, lsl #2]
                  strb r0, [r2]
                  ldr  sp, [r1, r0, lsl #2]
                  dsb
                  
                  pop  {r1}
                  msr  PSR, r1
                  
                  pop  {r0-r12, pc}
                  
                  ENDP
                  END