
#include "SerialTrace.h"
#include "MenuSelectorSimple.h"
#include "L298NController.h"
#include <EEPROM.h>

#define STATUS_SELECT        0
#define STATUS_RUNNING_WATCH 1
#define STATUS_RUNNING_MIX   2

#define STATUS_ADDRESS_ON_EEPROM 0

String selectOperationMenu[] =
{
  "Which program v9", // STATUS_SELECT
  " WatchWnd",        // STATUS_RUNNING_WATCH
  " Mixer",           // STATUS_RUNNING_MIX
  ""
};

unsigned char previousStatus;
unsigned long loopCount;
unsigned int  intervalBetweenMotorEngagementsInSeconds; 
unsigned int  motorEngagementDurationInSeconds; 
unsigned int  motorPower0to255;
bool keepRunning;
bool firstRun;
const int enA=11, in1=10;
unsigned long latestExecutionInSeconds;
L298NController l298NController(enA, in1);

void RunMotorIfNeeded() {
  unsigned long currentSecond = millis() / 1000;
  
  if (currentSecond < latestExecutionInSeconds) { // millis will overflow after ~50 days...
    latestExecutionInSeconds = 0;
  }
  
  unsigned long elapsedFromLatestExecution = currentSecond - latestExecutionInSeconds;
  
  serialSSI("RunMotorIfNeeded", "elapsedFromLatestExecution", elapsedFromLatestExecution);
  serialSSI("RunMotorIfNeeded", "intervalBetweenMotorEngagementsInSeconds", intervalBetweenMotorEngagementsInSeconds);
  MenuSelectorSimple::PrintAt(String(elapsedFromLatestExecution), 9, 1);
  MenuSelectorSimple::PrintAt(String(intervalBetweenMotorEngagementsInSeconds), 13, 1);
  
  if (firstRun || elapsedFromLatestExecution > intervalBetweenMotorEngagementsInSeconds) {
    MenuSelectorSimple::PrintAt("*", 15, 1);
    l298NController.Start(L298N_MOTOR_A, motorPower0to255, motorEngagementDurationInSeconds * 1000, keepRunning);
    latestExecutionInSeconds = millis() / 1000;
    MenuSelectorSimple::PrintAt("_", 15, 1);
  }
  
  if (!keepRunning) {
    digitalWrite(in1, LOW);
  }
  
  firstRun = false;
}


void setup() {
  firstRun = true;
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
  MenuSelectorSimple::Display();
  unsigned char currentStatus = EEPROM[STATUS_ADDRESS_ON_EEPROM];
  if (currentStatus == STATUS_SELECT)
  {
    l298NController.Stop(L298N_MOTOR_A);
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
    if (currentStatus == STATUS_RUNNING_WATCH) {
      intervalBetweenMotorEngagementsInSeconds = 15 * 60;
      motorEngagementDurationInSeconds = 1;
      motorPower0to255 = 128;
      keepRunning = false;
    }
    if (currentStatus == STATUS_RUNNING_MIX) {
      intervalBetweenMotorEngagementsInSeconds = 0;
      motorEngagementDurationInSeconds = 1;
      motorPower0to255 = 255;
      keepRunning = true;
    }
    RunMotorIfNeeded();
  }
  previousStatus = currentStatus;
  delay(500);                       // wait for 1/2 seconds
}
