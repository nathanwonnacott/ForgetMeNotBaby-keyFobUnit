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
  setLED((void*) 0);
  Serial.begin(9600);
  // put your setup code here, to run once:
  timerController = TimerOneMulti::getTimerController();
  stateMachine = CarseatUnitStateMachine::getStateMachine();
  
  Serial.println("Adding timer events");
  timerController->addEvent(5000000,setLED,false, (void*) 0);
  timerController->addEvent(4000000,setLED,false, (void*) 1);
  
}

void loop() {

  //Serial.print("Timer says ");
  //Serial.println(Timer1.read(),DEC);
}
