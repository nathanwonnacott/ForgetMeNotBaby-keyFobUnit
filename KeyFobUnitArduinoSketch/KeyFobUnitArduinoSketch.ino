#include <TimerOne.h>
#include "TimerOneMulti.h"
#include <avr/sleep.h>
#include <avr/power.h>


#include "KeyFobUnitStateMachine.h"

KeyFobUnitStateMachine* stateMachine;
TimerOneMulti* timerController;


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
  pinMode(BUZZER_PIN1, OUTPUT);
  pinMode(BUZZER_PIN2, OUTPUT);
  pinMode(BUTTON, INPUT);
  
  digitalWrite(LED1, LOW);
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
  if (interruptFlagsCopy & BUTTON_PRESS && (digitalRead(BUTTON) == HIGH))
  {
    if (odd)
      stateMachine->connectSound();
    else
      stateMachine->disconnectSound();
    
    odd = !odd;
  }

}

