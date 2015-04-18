#include <TimerOne.h>
#include "TimerOneMulti.h"


#include "CarseatUnitStateMachine.h"

CarseatUnitStateMachine* stateMachine;
TimerOneMulti* timerController;

//TODO remove debug callback
void setLED(void* arg)
{
  int val = (int) arg;
  digitalWrite(13,val);
}

void setup() {
  
  pinMode(13,OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(12,LOW);
  setLED((void*) 0);
  Serial.begin(9600);
  // put your setup code here, to run once:
  timerController = TimerOneMulti::getTimerController();
  stateMachine = CarseatUnitStateMachine::getStateMachine();
  
  //Serial.println("Adding timer events");
  timerController->addEvent(7000000,setLED,false, (void*) 0);
  timerController->addEvent(4000000,setLED,false, (void*) 1);
  timerController->addEvent(3000000,setLED,false, (void*) 0);
  TimerEvent* eventToCancel = timerController->addEvent(2000000,setLED,false, (void*) 1);
  timerController->addEvent(5000000,setLED,false, (void*) 0);
  timerController->addEvent(6000000,setLED,false, (void*) 1);
  timerController->addEvent(8000000,setLED,false, (void*) 1);
  timerController->addEvent(8500000,setLED,false, (void*) 0);
  timerController->addEvent(2100000,setLED,false, (void*) 0);
  timerController->addEvent(2200000,setLED,false, (void*) 1);
  
  timerController->cancelEvent(eventToCancel);
  
}

void loop() {

}
