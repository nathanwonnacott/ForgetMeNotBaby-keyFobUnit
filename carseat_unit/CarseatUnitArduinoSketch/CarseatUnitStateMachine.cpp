
#include "CarseatUnitStateMachine.h"

//Initialize static variables

CarseatUnitStateMachine* CarseatUnitStateMachine::singleton = NULL;

//Class methods

CarseatUnitStateMachine::CarseatUnitStateMachine()
{
}

CarseatUnitStateMachine* CarseatUnitStateMachine::getStateMachine()
{
  if(singleton == NULL)
    singleton = new CarseatUnitStateMachine();
  
  return singleton;
}

void CarseatUnitStateMachine::recieveMessage(char* message, int count)
{
  
}
