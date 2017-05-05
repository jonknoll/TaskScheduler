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

@file task_scheduler.h
@license MIT
@version 2.0.0

@description Simple Task Scheduler
   This is basically a set of timers and a very simple run-to-completion
   scheduler. The scheduler runs off the millis() counter so all delays are in
   milliseconds.
   Currently supported:
      - 256 tasks
      - 65535 ms max delay
   
   NOTE: This class only supports one built-in instance (TS).
   
   In your program you need to set up a task block array, where each entry is
   the starting parameters for a task. Here is an example of what should be at
   the top of your sketch. The enumeration and NUM_TASKS are optional, if you
   just want to use the actual numbers.
   
// Task list where each line matches a line in the task block
enum
{
   LED_BLINK_TASK = 0,
   HELLO_WORLD_TASK
};

// The task block with starting values
TaskBlock taskBlock[] = 
{
   // Entry Point    State     Starting Count
   {&ledBlinkTask,   TASK_READY, 0},  // LED_BLINK_TASK
   {&helloWorldTask, TASK_READY, 0}   // HELLO_WORLD_TASK
};

// The number of tasks (automatically calculated
#define NUM_TASKS (sizeof(taskBlock)/sizeof(TaskBlock))


Then in your setup():

setup()
{
   // put other setup stuff here
   void TS.initialize(taskBlock, NUM_TASKS);
}

loop()
{
   TS.run();
}

*******************************************************************************/
#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include "Arduino.h"

typedef enum
{
	TASK_DISABLED,	// inactive
	TASK_IDLE,	   // active but not running
	TASK_READY,	   // ready to run (or running)
} TaskState;

typedef void (*TaskFuncPtr)(void);

typedef struct
{
   TaskFuncPtr funcPtr; // function pointer -- the task entry point
   TaskState state;         // the current task state
   uint16_t timer;               // count down timer to the next task run
} TaskBlock;


class TaskScheduler
{
public:
   TaskScheduler(void);
   
   /**
    * @description Initialize the scheduler with the task block. Place this in
    *    the setup() function.
    * @param taskBlockPtr - pointer to the start of your task block
    * @param taskBlockSize - the number of tasks in the task block
    */
   void initialize(TaskBlock *taskBlockPtr, uint8_t numTasks);
   
   /**
    * @description Run the scheduler. Place this in the loop() function (it
    *    should be the only thing in the loop() function).
    */
   void run(void);
   
   /**
    * @description Add set a sleep time for the task.
    *    This sets the task's timer to the number of milliseconds the scheduler
    *    should wait before running again.
    *    NOTE: Depending on the run time of the tasks, the delay cannot be
    *    guaranteed. It might be longer than specified.
    * @param taskId - the task ID (array entry in the task block)
    * @param ms - number of milliseconds to sleep
    */
   void sleep(uint8_t taskId, uint16_t ms);
   
   /**
    * @description Set the entry point for the task.
    *    This is a more advanced function. It is for special cases where you
    *    want to change where the task starts from, the next time it runs.
    *    Normally you would build your task as a state machine (all in one
    *    function) but if you wanted to separate the states into different
    *    functions, you can modify the task's entry point on the fly.
    * @param taskId - the task ID (array entry in the task block)
    * @param newFuncPtr - new function pointer for the task
    */
   void setEntryPoint(uint8_t taskId, TaskFuncPtr newFuncPtr);
   
   /**
    * @description Get the task's state.
    *    Get the current state of a task.
    * @param taskId - the task ID (array entry in the task block)
    * @returns TS_taskState - the task's current state
    */
   TaskState getState(uint8_t taskId);
   
   /***************************************************************************//**
    * @description Set the task's state.
    *    Normally the scheduler automatically switches the state between
    *    TASK_IDLE and TASK_READY. But you can manually override what the
    *    scheduler is doing or you can disable the task altogether with
    *    TASK_DISABLE.
    * @param taskId - the task ID (array entry in the task block)
    * @param newState - new state for the task
    */
   void setState(uint8_t taskId, TaskState newState);
   
private:
   TaskBlock *ts_taskBlockPtr;
   uint8_t ts_taskBlockSize;
   uint32_t ts_prevMs;
};


#endif
