 
#include "TimerOneMulti.h"

//Initialize static variables

TimerOneMulti* TimerOneMulti::singleton = NULL;
volatile bool TimerOneMulti::timerFirstShot = false;

//TODO remove (just useful for debug purposes
int ledState = LOW;
void toggleLight()
{
  ledState = ! ledState;
  digitalWrite(13,ledState);
}

void quickBeep()
{
  for(unsigned long i=0;i<4000;i++)
  {
    digitalWrite(12,HIGH);
  }
  digitalWrite(12,LOW);
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

void TimerEvent::add(TimerEvent* event)
{
  if(next == NULL)
  {
    next = event;
  }
  else if(event->delta < next->delta)
  {
    //insert it here (and update previous next's delta)
    next->delta -= event->delta;
    event->next = next;
    next = event;
  }
  else
  {
    event->delta -= next->delta;
    next->add(event);
  }
}

//TimerOneMulti class methods

TimerOneMulti::TimerOneMulti()
{
  events = NULL;
  Timer1.initialize(MAX_PERIOD);         // initialize timer1

  
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
      else
      {
        event->delta -= events->delta;
        events->add(event);
      }
      
    }
    
  }
  interrupts();
  
  Timer1.setPeriod(events->delta);
  if(queueWasEmpty)
  {
    timerFirstShot = true;
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
     timerFirstShot = true;
     Timer1.setPeriod(events->delta);
     Timer1.start();
     Timer1.attachInterrupt(tick);
   } 
  }
  
}

void TimerOneMulti::tick()
{
 
 //When you start the timer, it generates an interrupt immediately and then continues
 //generating interrupts at the interval specified by period. 
 //But we don't want to do anything on the first shot, just on the second one
  if ( timerFirstShot)
  {
    timerFirstShot = false;
    return;
  }
    
  TimerOneMulti::getTimerController()->advanceTimer();
}

