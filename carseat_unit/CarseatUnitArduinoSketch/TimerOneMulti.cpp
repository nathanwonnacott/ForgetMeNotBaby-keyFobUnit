 
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

void quickBeep()
{
  for(unsigned long i=0;i<1000;i++)
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

//TimerOneMulti class methods

TimerOneMulti::TimerOneMulti()
{
  quickBeep();
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
      //Serial.println("Adding first event");
      events = event;
      queueWasEmpty = true;
    }
    else
    {
      //Serial.println("Adding non-first event");
      events->delta -= Timer1.read();
      if ( events->delta > period)
      {
        //Insert at the beginning of the list
        events->delta -= period;
        /*Serial.println("Adding at beginning of list");
        Serial.print("Delta to next event is ");
        Serial.println(events->delta, DEC);*/
        event->next = events;
        events  = event;
      }
      
    }
    
  }  
  //Serial.println("Enabling interrupts");
  interrupts();
  //Serial.println("Interrupts enabled");
  
  Timer1.setPeriod(events->delta);
  if(queueWasEmpty)
  {
    //Serial.println("Starting timer");
    Timer1.start();
    Timer1.attachInterrupt(tick);  // attaches callback() as a timer overflow interrupt
  }
  
  //TODO figure out if any timer interrupts should have occurred while we were in the critical section
}

void TimerOneMulti::advanceTimer()
{
  /*Serial.print("Events = 0x");
  Serial.println((int)events,HEX);
  Serial.print("Time since last timer rollover =");
  Serial.println(Timer1.read(),DEC);*/
  
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
     /*Serial.print("Next event will occur in ");
     Serial.println(events->delta,DEC);*/
     Timer1.setPeriod(events->delta);
     Timer1.start();
     Timer1.attachInterrupt(tick);
   } 
  }
  
}

void TimerOneMulti::tick()
{
  quickBeep();
  //Serial.println("Tick!");
  
  //Timer should read 0, but for whatever reason when I first start the timer
  //I get an interrupt shortly after. When the proper interrupt occurs, the timer
  //should read 0, (but we check for less than 100 here just in case there's som
  //situation in which the time keeps advancing and there's a small delay.
  if(Timer1.read() > 100)
  {
    //Serial.println("Bad tick");
    //Serial.println(Timer1.read(),DEC);
    return;
  }
  TimerOneMulti::getTimerController()->advanceTimer();
}

