#include <TimerOne.h>


#include "CarseatUnitStateMachine.h"

CarseatUnitStateMachine* stateMachine;

void setup() {
  // put your setup code here, to run once:
  stateMachine = CarseatUnitStateMachine::getStateMachine();
}

void loop() {
  // put your main code here, to run repeatedly:

}
