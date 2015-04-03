
#include "CarseatUnitStateDiagram.h";

CarseatUnitStateDiagram::CarseatUnitStateDiagram(unsigned short id)
{
  this->id = id;
  
  pinMode(4,OUTPUT);
  digitalWrite(4, HIGH);
}
