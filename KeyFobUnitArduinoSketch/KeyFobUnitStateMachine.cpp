
#include "KeyFobUnitStateMachine.h"

//Initialize static variables

KeyFobUnitStateMachine* KeyFobUnitStateMachine::singleton = NULL;
volatile unsigned char KeyFobUnitStateMachine::interruptFlags = 0;

//Class methods

KeyFobUnitStateMachine::KeyFobUnitStateMachine()
{
  timerController = TimerOneMulti::getTimerController();
  serialPort = new SoftwareSerial(RX, TX);
  serialPort->begin(9600);
  state = DISCONNECTED;
  heartBeatWaitTimer = NULL;
  connectionRetryTimer = NULL;
  connectionRetryCount = 0;
}

SoftwareSerial* KeyFobUnitStateMachine::getSerialPort()
{
  return serialPort;
}

KeyFobUnitStateMachine* KeyFobUnitStateMachine::getStateMachine()
{
  if(singleton == NULL)
    singleton = new KeyFobUnitStateMachine();
  
  return singleton;
}

void KeyFobUnitStateMachine::receiveMessage(char* message, int count)
{
  
  if ( count == strlen("FMNB:SeatDown\n") && strncmp(message,"FMNB:SeatDown\n",count) )
  {
    receivedSeatDownMessage();
  }
  else if( count == strlen("FMNB:SeatUp\n") && strncmp(message,"FMNB:SeatUp\n",count) )
  {
    receivedSeatUpMessage();
  }
}

void KeyFobUnitStateMachine::receivedSeatDownMessage()
{
  if ( state != CONNECTED)
  {
    connectSound();
  }
  
  if ( state == CONNECTION_RETRY)
  {
    cancelReconnectTimer();
  }
  
  if ( heartBeatWaitTimer != NULL)
  {
    timerController->cancelEvent(heartBeatWaitTimer);
    heartBeatWaitTimer = NULL;
  }
  heartBeatWaitTimer = timerController->addEvent(HEART_BEAT_WAIT_TIMEOUT,timerISR, false, (void*) HEART_BEAT_WAIT_TIMER);
  state = CONNECTED;
  digitalWrite(LED1,HIGH);
}

void KeyFobUnitStateMachine::receivedSeatUpMessage()
{
  if ( state != DISCONNECTED)
  {
    disconnectSound();
  }
  if ( state == CONNECTION_RETRY)
  {
    cancelReconnectTimer();
  }
  if ( heartBeatWaitTimer != NULL)
  {
    timerController->cancelEvent(heartBeatWaitTimer);
    heartBeatWaitTimer = NULL;
  }
  state = DISCONNECTED;
  digitalWrite(LED1,LOW);
}


void KeyFobUnitStateMachine::heartBeatWaitTimerExpired()
{
  state = CONNECTION_RETRY;
  digitalWrite(LED1,LOW);
  
  connectionRetryTimer = timerController->addEvent(CONNECTION_RETRY_TIMEOUT, timerISR, true, (void*) CONNECTION_RETRY_TIMER);
  serialPort->println("FMNB:Reconnect");
  
}

void KeyFobUnitStateMachine::connectionRetryTimeout()
{
  if( ++connectionRetryCount > MAX_NUM_CONNECTION_RETRIES)
  {
    alarmSound();
  }
  serialPort->println("FMNB:Reconnect");
}

void KeyFobUnitStateMachine::cancelReconnectTimer()
{
  if (connectionRetryTimer != NULL)
  {
    timerController->cancelEvent(connectionRetryTimer);
    connectionRetryTimer = NULL;
    connectionRetryCount = 0;
  }
}

void KeyFobUnitStateMachine::connectSound()
{
  buzzerSet(HIGH, LOW);
  delay(100);
  buzzerSet(HIGH, HIGH);
  delay(100);
  buzzerSet(LOW);
}
  
void KeyFobUnitStateMachine::disconnectSound()
{
  buzzerSet(HIGH, HIGH);
  delay(100);
  buzzerSet(HIGH, LOW);
  delay(100);
  buzzerSet(LOW);
}
  
void KeyFobUnitStateMachine::alarmSound()
{
  cancelReconnectTimer();
  state = ALARMING;
  buzzerSet(HIGH,HIGH);
  delay(1000);
  buzzerSet(LOW);
}

void KeyFobUnitStateMachine::buzzerSet(int val, int pitch)
{
  digitalWrite(BUZZER_PIN1, val);
  if(val == LOW || pitch == HIGH)
  {
    digitalWrite(BUZZER_PIN2, val);
  }
  if(val == HIGH && pitch == LOW)
  {
    digitalWrite(BUZZER_PIN2, LOW);
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


