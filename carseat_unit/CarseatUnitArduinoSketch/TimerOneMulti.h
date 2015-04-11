
/*
 * A wrapper around the TimerOne library that allows for setting up multiple timers using one hardware timer.
 * Also allows for timers with delays longer than 2^16 ticks
*/

#ifndef TIMER_ONE_MULTI_H
  #define TIMER_ONE_MULTI_H

#include "Arduino.h"
#include<TimerOne.h>

#define MAX_PERIOD 8388608

class TimerEvent
{
  friend class TimerOneMulti;
private:
  TimerEvent(unsigned long delta, void (*callback) (void*), void* arg);
  unsigned long delta;
  void (*callback) (void*);
  void* arg;  
  TimerEvent* next;
};

class TimerOneMulti
{
private:
  TimerEvent* events;
  static TimerOneMulti* singleton;
  
public:
  static TimerOneMulti* getTimerController();
  
  TimerEvent* addEvent(unsigned long period, void (*callback) (void*), void* arg = NULL);
  
  /*
   This is really the tick function. The actual tick function has to be static, so it calls this one with the singleton.
   This one has access to the private members.
   This shouldn't really be used as a public method, it should only be called by tick()
  */
  void advanceTimer();
  

private:
  TimerOneMulti();
  
  static void tick();
  
};
  
#endif

