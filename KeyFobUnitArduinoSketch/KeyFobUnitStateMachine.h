#include "Arduino.h"
#include "TimerOneMulti.h"

/*
 This class represents a state machine for the Pressure sensing unit in the carseat.
 This is the main controller for this program.
*/

#ifndef KEY_FOB_UNIT_STATE_MACHINE_H
  #define KEY_FOB_UNIT_STATE_MACHINE_H
 
  
#define ID 0x1234

//ISR flag values
#define BUZZER_HIGH_START_TIMER  0b10000000
#define BUZZER_LOW_START_TIMER   0b01000000
#define BUZZER_STOP_TIMER        0b00100000
#define BUTTON_PRESS             0b00010000

#define BUTTON_IRQ  0  //use 0 for Uno, I believe we need 1 for Nano

//PIN definitions
#define LED1         13  //Not sure if we need an LED in production, but it helps for debugging
#define BUZZER_PIN1  11
#define BUZZER_PIN2  10
#define BUTTON       2
#define TX           1
#define RX           0

//Timeout values

enum KeyFobState
{
};

class KeyFobUnitStateMachine
{
public:
  //Flags indicating which interrupts have occurred. Should not be accessed with interrupts enabled in case later I decide to make it
  //a datatype that requires multiple instructions to read
  //It really doesn't belong inside the class, but for some reason I keep getting multiple definition errors when I bring it outside
  volatile static unsigned char interruptFlags;
  
private:
  static KeyFobUnitStateMachine* singleton;
  TimerOneMulti* timerController;

  KeyFobState state;
  TimerEvent* seatUpWaitTimer;
  
public:
  static KeyFobUnitStateMachine* getStateMachine();
  
  void recieveMessage(char* message, int count);
  
  void seatStatusChange(int val);
  
  void seatUpWaitTimerExpired();
  
  void connectSound();
  
  void disconnectSound();
  
  void alarmSound();
  
  void buzzerSet(int val, int pitch = LOW);

private:  
  KeyFobUnitStateMachine();
  
};

//ISRs
void timerISR(void* timerType);  //This is called by TimerOne library, so it can take an argument
void buttonPressISR();


#endif