/***************************************************************************//**
MIT License

Copyright (c) 2017 Jon Knoll

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@file task_scheduler.cpp
@description see task_scheduler.h
*******************************************************************************/
#include "Arduino.h"
#include "TaskScheduler.h"


TaskScheduler::TaskScheduler(void)
{
   ts_taskBlockPtr = 0;
   ts_taskBlockSize = 0;
   ts_prevMs = 0;
}

// put this in setup()
void TaskScheduler::initialize(TaskBlock *taskBlockPtr, uint8_t numTasks)
{
   ts_taskBlockPtr = taskBlockPtr;
   ts_taskBlockSize = numTasks;
   ts_prevMs = millis();
}

// put this in loop()
void TaskScheduler::run(void)
{
   uint8_t i;
   TaskBlock *currentTaskPtr;
   uint32_t currentMs;
   uint32_t msDiff;
   
   currentMs = millis();
   msDiff = currentMs - ts_prevMs;
   
   for (i = 0; i < ts_taskBlockSize; i++)
   {
      currentTaskPtr = &ts_taskBlockPtr[i];
      
      if(currentTaskPtr->state == TASK_IDLE)
      {
         if(currentTaskPtr->timer > 0)
         {
            if(currentTaskPtr->timer > msDiff)
            {
               currentTaskPtr->timer -= msDiff;
            }
            else
            {
               currentTaskPtr->timer = 0;
            }
         }

         if(currentTaskPtr->timer == 0)
         {
            currentTaskPtr->state = TASK_READY;
         }
      }
      
      if(currentTaskPtr->state == TASK_READY)
      {
         // task gets run here...
         currentTaskPtr->funcPtr();
         currentTaskPtr->state = TASK_IDLE;
      }
   }
   ts_prevMs = currentMs;
}

void TaskScheduler::sleep(uint8_t taskId, uint16_t ms)
{
   ts_taskBlockPtr[taskId].timer = ms;
}

void TaskScheduler::setEntryPoint(uint8_t taskId, TaskFuncPtr newFuncPtr)
{
   ts_taskBlockPtr[taskId].funcPtr = newFuncPtr;
}

TaskState TaskScheduler::getState(uint8_t taskId)
{
   return(ts_taskBlockPtr[taskId].state);
}

void TaskScheduler::setState(uint8_t taskId, TaskState newState)
{
   ts_taskBlockPtr[taskId].state = newState;
}
