
#include "KeyFobUnitStateMachine.h"

//Initialize static variables

KeyFobUnitStateMachine* KeyFobUnitStateMachine::singleton = NULL;
volatile unsigned char KeyFobUnitStateMachine::interruptFlags = 0;

//Class methods

KeyFobUnitStateMachine::KeyFobUnitStateMachine()
{
  timerController = TimerOneMulti::getTimerController();
}

KeyFobUnitStateMachine* KeyFobUnitStateMachine::getStateMachine()
{
  if(singleton == NULL)
    singleton = new KeyFobUnitStateMachine();
  
  return singleton;
}

void KeyFobUnitStateMachine::receiveMessage(char* message, int count)
{
  buzzerSet(HIGH,HIGH);
  delay(10);
  buzzerSet(LOW);
}

void KeyFobUnitStateMachine::connectSound()
{
  buzzerSet(HIGH, LOW);
  timerController->addEvent(100000, timerISR, false, (void*) BUZZER_STOP_TIMER);
  timerController->addEvent(140000, timerISR, false, (void*) BUZZER_HIGH_START_TIMER);
  timerController->addEvent(200000, timerISR, false, (void*) BUZZER_STOP_TIMER);
}
  
void KeyFobUnitStateMachine::disconnectSound()
{
  buzzerSet(HIGH, HIGH);
  timerController->addEvent(100000, timerISR, false, (void*) BUZZER_STOP_TIMER);
  timerController->addEvent(140000, timerISR, false, (void*) BUZZER_LOW_START_TIMER);
  timerController->addEvent(200000, timerISR, false, (void*) BUZZER_STOP_TIMER);
}
  
void KeyFobUnitStateMachine::alarmSound()
{
}

void KeyFobUnitStateMachine::buzzerSet(int val, int pitch)
{
  digitalWrite(BUZZER_PIN1, val);
  if(val == LOW || pitch == HIGH)
  {
    digitalWrite(BUZZER_PIN2, val);
  }
}

//ISRs
void timerISR(void* timerType)
{
  KeyFobUnitStateMachine::interruptFlags |= (unsigned char) (int) timerType;
}

void buttonPressISR()
{
  KeyFobUnitStateMachine::interruptFlags |= BUTTON_PRESS;
}


