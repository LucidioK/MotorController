
#include "SerialTrace.h"
#include "MenuSelectorSimple.h"
#include <EEPROM.h>

#define STATUS_SELECT        0
#define STATUS_RUNNING_WATCH 1
#define STATUS_RUNNING_MIX   2

#define STATUS_ADDRESS_ON_EEPROM 0

String selectOperationMenu[] =
{
  "Which program v3", // STATUS_SELECT
  " Watch wind",      // STATUS_RUNNING_WATCH
  " Mixer",           // STATUS_RUNNING_MIX
  ""
};

unsigned char previousStatus;
unsigned long loopCount;
unsigned int  intervalBetweenMotorEngagementsInSeconds; 
unsigned int  motorEngagementDurationInSeconds; 
unsigned int  motorPower0to255;
bool keepRunning;
const int enA=2, in1=3;
unsigned long latestExecutionInSeconds;

void RunMotorIfNeeded() {
  unsigned long currentSecond = millis() / 1000;
  if (currentSecond < latestExecutionInSeconds) { // millis will overflow after ~50 days...
    latestExecutionInSeconds = 0;
  }
  unsigned long elapsedFromLatestExecution = currentSecond - latestExecutionInSeconds;
  serialSSI("RunMotorIfNeeded", "elapsedFromLatestExecution", elapsedFromLatestExecution);
  serialSSI("RunMotorIfNeeded", "intervalBetweenMotorEngagementsInSeconds", intervalBetweenMotorEngagementsInSeconds);

  if (elapsedFromLatestExecution > intervalBetweenMotorEngagementsInSeconds) {
    digitalWrite(in1, HIGH);
    analogWrite(enA, motorPower0to255);
    delay(motorEngagementDurationInSeconds * 1000);

    latestExecutionInSeconds = millis() / 1000;
  }
  if (!keepRunning) {
    digitalWrite(in1, LOW);
  }
}


void setup() {
  previousStatus = STATUS_SELECT;
  loopCount = intervalBetweenMotorEngagementsInSeconds = motorEngagementDurationInSeconds = motorPower0to255 = latestExecutionInSeconds = 0;
  keepRunning = false;
  Serial.begin(9600);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);  
  digitalWrite(in1, LOW);
  analogWrite(enA, 0);
}



void loop() {
  serialSSI("loop", "-----------", millis());
  
  unsigned char currentStatus = EEPROM[STATUS_ADDRESS_ON_EEPROM];
  if (currentStatus == STATUS_SELECT)
  {
    for (int i = 0; i < 10; i++) {
      digitalWrite(in1, HIGH);
      delay(50);      
      digitalWrite(in1, LOW);
      delay(50);
    }
    MenuSelectorSimple ms = MenuSelectorSimple(selectOperationMenu);
    int pos = ms.Select();
    EEPROM[STATUS_ADDRESS_ON_EEPROM] = pos;
    MenuSelectorSimple::PrintAt(selectOperationMenu[pos], 0, 0);
  }
  else
  {
    String runningText = "Running ";
    runningText += (millis()/1000);
    MenuSelectorSimple::PrintAt(runningText, 0, 0);
    MenuSelectorSimple::PrintAt(selectOperationMenu[currentStatus], 0, 1);
    if (MenuSelectorSimple::AnyButtonHit()) {
      EEPROM[STATUS_ADDRESS_ON_EEPROM] = STATUS_SELECT;
    }
    //if (previousStatus != currentStatus) {
      if (currentStatus == STATUS_RUNNING_WATCH) {
        intervalBetweenMotorEngagementsInSeconds = 60;
        motorEngagementDurationInSeconds = 1;
        motorPower0to255 = 125;
        keepRunning = false;
      }
      if (currentStatus == STATUS_RUNNING_MIX) {
        intervalBetweenMotorEngagementsInSeconds = 0;
        motorEngagementDurationInSeconds = 1;
        motorPower0to255 = 255;
        keepRunning = true;
      }
      RunMotorIfNeeded();
    //}
  }
  previousStatus = currentStatus;
  delay(500);                       // wait for 1/2 seconds
}
