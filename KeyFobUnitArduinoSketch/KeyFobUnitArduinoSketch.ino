#include <TimerOne.h>
#include <SoftwareSerial.h>
#include "TimerOneMulti.h"
#include <avr/sleep.h>
#include <avr/power.h>


#include "KeyFobUnitStateMachine.h"

//Declare global variables
KeyFobUnitStateMachine* stateMachine;
TimerOneMulti* timerController;

char serialReceiveBuffer[MESSAGE_BUFFER_SIZE];

void sleep()
{
  /* Disable all of the unused peripherals. This will reduce power
   * consumption further and, more importantly, some of these
   * peripherals may generate interrupts that will wake our Arduino from
   * sleep!
   * Don't dissable spi because we want that to wake us up
   */
  power_adc_disable();
  power_timer0_disable();
  power_timer2_disable();
  power_twi_disable();

  /* Now enter sleep mode. */
  sleep_mode();

  /* The program will continue from here after the timer timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */

  /* Re-enable the peripherals. */
  power_all_enable();
}
void setup() {

  //Set up pins
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER_PIN1, OUTPUT);
  pinMode(BUZZER_PIN2, OUTPUT);
  pinMode(BUTTON, INPUT);
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(BUZZER_PIN1, LOW);
  digitalWrite(BUZZER_PIN2, LOW);
  
  //Set up serial
  Serial.begin(9600);

  //Set up objects
  timerController = TimerOneMulti::getTimerController();
  stateMachine = KeyFobUnitStateMachine::getStateMachine();

  //Set up interrupts
  attachInterrupt(BUTTON_IRQ, buttonPressISR, RISING);
  
  //Enable sleeping
  //We can't go any deeper than IDLE, otherwise Timer1 won't wake us up
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

}

bool odd = true;

void loop()
{
  sleep();
  unsigned char interruptFlagsCopy = 0;

  noInterrupts();
  interruptFlagsCopy = KeyFobUnitStateMachine::interruptFlags;
  KeyFobUnitStateMachine::interruptFlags = 0;
  interrupts();
  
  /*if ( interruptFlagsCopy != 0)
  {
    char buff[13];
    buff[0] = 'F';
    buff[1] = ':';
    buff[2] = '0';
    buff[3] = 'x';
    buff[4] = (interruptFlagsCopy & 0b10000000)? '1':'0';
    buff[5] = (interruptFlagsCopy & 0b01000000)? '1':'0';
    buff[6] = (interruptFlagsCopy & 0b00100000)? '1':'0';
    buff[7] = (interruptFlagsCopy & 0b00010000)? '1':'0';
    buff[8] = (interruptFlagsCopy & 0b00001000)? '1':'0';
    buff[9] = (interruptFlagsCopy & 0b00000100)? '1':'0';
    buff[10] = (interruptFlagsCopy & 0b00000010)? '1':'0';
    buff[11] = (interruptFlagsCopy & 0b00000001)? '1':'0';
    buff[12] = '\0';
    Serial.println(buff);
  }*/
  
  if ( interruptFlagsCopy & BUZZER_HIGH_START_TIMER)
  {
    stateMachine->buzzerSet(HIGH, HIGH);
  }
  if ( interruptFlagsCopy & BUZZER_LOW_START_TIMER)
  {
    stateMachine->buzzerSet(HIGH, LOW);
  }
  if ( interruptFlagsCopy & BUZZER_STOP_TIMER)
  {
    stateMachine->buzzerSet(LOW);
  }
  if ( interruptFlagsCopy & HEART_BEAT_WAIT_TIMER)
  {
    stateMachine->heartBeatWaitTimerExpired();
  }
  if ( interruptFlagsCopy & CONNECTION_RETRY_TIMER)
  {
    stateMachine->connectionRetryTimeout();
  }
  if (interruptFlagsCopy & ALARM_PHASE_TIMER)
  {
    stateMachine->alarmSound();
  }
  if (interruptFlagsCopy & BUTTON_PRESS)
  {
    stateMachine->cancelReconnectTimer();
    stateMachine->cancelAlarm();
  }
  
  while(stateMachine->getSerialPort()->available())
  {
    byte bytesRead = stateMachine->getSerialPort()->readBytesUntil('\n',serialReceiveBuffer,MESSAGE_BUFFER_SIZE);
    /*serialReceiveBuffer[bytesRead] = '\0';
    Serial.println(serialReceiveBuffer);*/
    stateMachine->receiveMessage(serialReceiveBuffer,bytesRead);
  }
  
  digitalWrite(LED2,! digitalRead(LED2));

}

