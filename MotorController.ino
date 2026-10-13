
#include "SerialTrace.h"
#include "MenuSelectorSimple.h"
#include "L298NController.h"
#include <EEPROM.h>

#define STATUS_SELECT         0
#define STATUS_RUNNING_WATCH  1
#define STATUS_RUNNING_MIX    2
#define STATUS_RUNNING_RANDOM 3
#define STATUS_RUNNING_AHEAD_REVERSE 4
#define STATUS_RUNNING_REVERSE_ONLY 5

#define STATUS_ADDRESS_ON_EEPROM 0

String selectOperationMenu[] =
{
  "Which program vD", // STATUS_SELECT
  " WatchWnd",        // STATUS_RUNNING_WATCH
  " Mixer",           // STATUS_RUNNING_MIX
  " Random",          // STATUS_RUNNING_RANDOM
  " AheadRvrs",       // STATUS_RUNNING_AHEAD_REVERSE
  " Reverse",         // STATUS_RUNNING_REVERSE_ONLY
  ""
};

unsigned char previousStatus;
unsigned long loopCount;
unsigned int  intervalBetweenMotorEngagementsInSeconds; 
unsigned int  motorEngagementDurationInSeconds; 
unsigned int  motorPower0to255;
bool keepRunning;
bool firstRun;
const int in1=2, enA=11, in2 = 12, csA = 13;
unsigned long latestExecutionInSeconds;
int watchProgram[] = { 1000, 0, -1000, 0 };
const int watchProgramCount = 4;
int mixProgram[] = { 1000 };
const int mixProgramCount = 1;
int reverseProgram[] = { -1000 };
const int reverseProgramCount = 1;
int aheadReverseProgram[] = { 1000, 0, -1000, 0 };
const int aheadReverseProgramCount = 4;
int randomProgram[8];
const int randomProgramCount = 8;


L298NController l298NController(in1, enA, in2, csA);

// Program is a sequence of time (in ms). 
//  If a value is positive, it will run the motor.
//  If a value is zero, it will stop the motor.
//  If a value is negative, it will run the motor in reverse.
// 
void RunMotorIfNeeded(int *program, int programSize) {
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
    for (int i =0; i < programSize; i++) {
      if (program[i]) {
        l298NController.Start(L298N_MOTOR_A, 255, program[i]);
      } else {
        l298NController.Stop(L298N_MOTOR_A);
      }
    }
    latestExecutionInSeconds = millis() / 1000;
    MenuSelectorSimple::PrintAt("_", 15, 1);
  }

  firstRun = false;
}


void setup() {
  firstRun = true;
  previousStatus = STATUS_SELECT;
  loopCount = intervalBetweenMotorEngagementsInSeconds = motorEngagementDurationInSeconds = motorPower0to255 = latestExecutionInSeconds = 0;
  keepRunning = false;
  Serial.begin(9600);
  for (int i = 4; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  randomSeed(analogRead(0));
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
    
    int* program;
    int  programCount;

    switch (currentStatus) {
      case STATUS_RUNNING_WATCH:
        intervalBetweenMotorEngagementsInSeconds = 5 * 60;
        program = watchProgram;
        programCount = watchProgramCount;
        break;
      case STATUS_RUNNING_MIX:
        intervalBetweenMotorEngagementsInSeconds = 0;
        program = mixProgram;
        programCount = mixProgramCount;
        break;
      case STATUS_RUNNING_RANDOM:
        intervalBetweenMotorEngagementsInSeconds = random(2, 10);
        for (int i =0; i < randomProgramCount; i++) {
          randomProgram[i] = (i % 2) ? 0 : 2000 - random(0, 4000);
        }
        program = randomProgram;
        programCount = randomProgramCount;
        break;
      case STATUS_RUNNING_AHEAD_REVERSE:
        program = aheadReverseProgram;
        programCount = aheadReverseProgramCount;
        break;
      case STATUS_RUNNING_REVERSE_ONLY:
        program = reverseProgram;
        programCount = reverseProgramCount;
        break;
    }
    RunMotorIfNeeded(program, programCount);
  }
  previousStatus = currentStatus;
  delay(500);                       // wait for 1/2 seconds
}
