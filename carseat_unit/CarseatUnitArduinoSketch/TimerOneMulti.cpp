 
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
TimerEvent::TimerEvent(unsigned long period, void (*callback) (void*), bool periodic, void* arg)
{
  this->period = period;
  this->delta = period;
  this->callback = callback;
  this->periodic = periodic;
  this->arg = arg;
  this->next = NULL;
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

TimerEvent* TimerOneMulti::addEvent(unsigned long period, void (*callback) (void*), bool periodic /* Default = false */, void* arg /* Default = NULL */)
{
  
  TimerEvent* event = new TimerEvent(period, callback, periodic, arg);
  bool queueWasEmpty = false;
  
  //Critical section
  noInterrupts();
  {  
    //Add to events
    if ( events ==   NULL)
    {
      events = event;
      queueWasEmpty = true;
    }
    else
    {
      events->delta -= Timer1.read();
      if ( events->delta > period)
      {
        //Insert at the beginning of the list
        events->delta -= period;
        event->next = events;
        events  = event;
      }
      
    }
    
  }  
  interrupts();
  
  Timer1.setPeriod(events->delta);
  if(queueWasEmpty)
  {
    Timer1.start();
    Timer1.attachInterrupt(tick);  // attaches callback() as a timer overflow interrupt
  }
  
  //TODO figure out if any timer interrupts should have occurred while we were in the critical section
}

void TimerOneMulti::advanceTimer()
{
  if (events != NULL)
  {
    events->callback(events->arg);
    events = events->next;
   if (events == NULL)
   {
     Timer1.stop();
   }
   else
   {
     Timer1.setPeriod(events->delta);
     Timer1.start();
     Timer1.attachInterrupt(tick);
   } 
  }
  
}

void TimerOneMulti::tick()
{
  TimerOneMulti::getTimerController()->advanceTimer();
}

