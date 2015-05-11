
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
  alarmPhaseTimer = NULL;
  connectionRetryCount = 0;
  alarmPhase = 0;
  
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
  if ( state != CONNECTED && state != CONNECTION_RETRY )
  {
    connectSound();
  }
  
  if ( state == CONNECTION_RETRY || state == ALARMING)
  {
    cancelReconnectTimer();
    cancelAlarm();
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
  if ( state == CONNECTION_RETRY || state == ALARMING)
  {
    cancelReconnectTimer();
    cancelAlarm();
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
  //Serial.println("reconnect");
  if( ++connectionRetryCount > MAX_NUM_CONNECTION_RETRIES && state != ALARMING)
  {
    state = ALARMING;
    alarmSound();
  }
  serialPort->println("FMNB:Reconnect");
}

void KeyFobUnitStateMachine::cancelAlarm()
{
  state = DISCONNECTED;
  if ( alarmPhaseTimer != NULL)
  {
    timerController->cancelEvent(alarmPhaseTimer);
    alarmPhaseTimer = NULL;
    alarmPhase = 0;
  }
  buzzerSet(LOW);
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
  /*char buff[9];
  buff[0] = 'A';
  buff[1] = 'l';
  buff[2] = 'a';
  buff[3] = 'r';
  buff[4] = 'm';
  buff[5] = ':';
  buff[6] = '0' + alarmPhase/10;
  buff[7] = '0' + alarmPhase%10;
  buff[8] = '\0';
  Serial.println(buff);*/
  
  
  if ( state != ALARMING)
  {
    return;
  }
  
  switch(alarmPhase)
  {
    case 0:
      if ( alarmPhaseTimer == NULL)
        alarmPhaseTimer = timerController->addEvent(ALARM_PHASE_TIMER_INTERVAL, timerISR, true, (void*) ALARM_PHASE_TIMER);
      
      /*for(int i=0;i<2;i++)
      {
      
        buzzerSet(HIGH,LOW);
        delay(40);
        buzzerSet(LOW);
        if ( i < 4) //Cut off the last delay so that we get back to the loop before the next tick
          delay(10);
      }*/
      buzzerSet(HIGH,LOW);
      break;
    case 1:
    case 3:
    case 5:
      buzzerSet(HIGH,HIGH);
      break;
    case 2:
    case 4:
    case 6:
     buzzerSet(HIGH,LOW);
     break;
    default:
      buzzerSet(LOW);
      alarmPhase = -1;
     
  }
  
  alarmPhase++;
}

void KeyFobUnitStateMachine::buzzerSet(int val, int pitch)
{
  digitalWrite(BUZZER_PIN1, val);
  if(val == HIGH && pitch == HIGH)
  {
    digitalWrite(BUZZER_PIN2, HIGH);
  }
  else
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


