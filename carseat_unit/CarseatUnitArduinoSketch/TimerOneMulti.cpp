
#include "TimerOneMulti.h"

//Initialize static variables

TimerOneMulti* TimerOneMulti::singleton = NULL;


//TODO remove (just useful for debug purposes
int ledState = LOW;
void toggleLight()
{
  ledState = ! ledState;
  digitalWrite(13,ledState);
}

//TimerEvent class methods
TimerEvent::TimerEvent(unsigned long delta, void (*callback) (void*), void* arg)
{
}

//TimerOneMulti class methods

TimerOneMulti::TimerOneMulti()
{
  pinMode(13,OUTPUT);
  
  toggleLight();
  events = NULL;
  //Timer1.initialize(MAX_PERIOD);         // initialize timer1, and set a 1/2 second period
  Timer1.initialize(500000);

  
}

TimerOneMulti* TimerOneMulti::getTimerController()
{
  if(singleton == NULL)
    singleton = new TimerOneMulti();
  
  return singleton;
}

TimerEvent* TimerOneMulti::addEvent(unsigned long period, void (*callback) (void*), void* arg /* Default = NULL */)
{
  TimerEvent* event = new TimerEvent(period, callback, arg);
  Timer1.setPeriod(period);
  
  //Add to events
  //TODO use events like a linked list
  events = event;
  
  Timer1.start();
  Timer1.attachInterrupt(tick);  // attaches callback() as a timer overflow interrupt
}

void TimerOneMulti::advanceTimer()
{
  events->callback(events->arg);
}

void TimerOneMulti::tick()
{
  TimerOneMulti::getTimerController()->advanceTimer();
}

