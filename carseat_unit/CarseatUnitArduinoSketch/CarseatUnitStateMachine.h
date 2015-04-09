#include <TimerOne.h>

/*
 This class represents a state machine for the Pressure sensing unit in the carseat.
 This is the main controller for this program.
*/

#include "Arduino.h"

#ifndef CARSEAT_UNIT_STATE_MACHINE_H
  #define CARSEAT_UNIT_STATE_MACHINE_H
  
#define ID 0x1234

class CarseatUnitStateMachine
{
  //shorts are 16 bit on all arduino boards
private:
  static CarseatUnitStateMachine* singleton;
  
public:
  static CarseatUnitStateMachine* getStateMachine();
  
  void recieveMessage(char* message, int count);

private:  
  CarseatUnitStateMachine();
  
};


#endif
