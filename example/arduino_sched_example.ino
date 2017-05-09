/***************************************************************************//**
Example program using the task_scheduler library.

This example was targeted for the Arduino UNO or NANO. It simultaneously blinks
the onboard LED and prints to the serial monitor, both at different rates. It
is a simple demonstration of how you can split your code into separate tasks
and control them independently with very little overhead.
*******************************************************************************/

#include <Arduino.h>
#include "TaskScheduler.h"

// Task setup
enum
{
   LED_BLINK_TASK = 0,
   HELLO_WORLD_TASK
};

static void ledBlinkTask(void);
static void helloWorldTask(void);

TaskBlock taskBlock[] = 
{
   {&ledBlinkTask, TASK_PEND, 5000},   // LED_BLINK_TASK (start after 5 seconds)
   {&helloWorldTask, TASK_RUN, 0}      // HELLO_WORLD_TASK (start now)
};

#define TASK_BLOCK_SIZE (sizeof(taskBlock)/sizeof(TaskBlock))

// create a task scheduler
TaskScheduler TS;

//////////////////
// LED BLINK TASK
//////////////////
const int ledPin = 13;
boolean ledState = LOW;

static void ledBlinkInit(void)
{
   // initialize digital pin 13 (LED) as an output.
   pinMode(ledPin, OUTPUT);
}

static void ledBlinkTask(void)
{
   if(ledState == LOW)
   {
      digitalWrite(ledPin, HIGH);
      ledState = HIGH;
      TS.sleep(LED_BLINK_TASK, 100);
   }
   else
   {
      digitalWrite(ledPin, LOW);
      ledState = LOW;
      TS.sleep(LED_BLINK_TASK, 700);
   }
}


////////////////////
// HELLO WORLD TASK
////////////////////
uint8_t count = 0;

static void helloWorldInit(void)
{
   // initialize serial communication:
   Serial.begin(9600);
}

static void helloWorldTask(void)
{
   Serial.print("Hello World! ");
   Serial.println(count++);
   TS.sleep(HELLO_WORLD_TASK, 500);
}



//The setup function is called once at startup of the sketch
void setup()
{
   ledBlinkInit();
   helloWorldInit();
   TS.initialize(&taskBlock[0], TASK_BLOCK_SIZE);
}

// The loop function is called in an endless loop
void loop()
{
   TS.run();
}
