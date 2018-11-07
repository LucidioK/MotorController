
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

void setup() {
  previousStatus = EEPROM[STATUS_ADDRESS_ON_EEPROM];
  Serial.begin(9600);
}



void loop() {
  serialSSI("loop", "-----------", millis());
  unsigned char currentStatus = EEPROM[STATUS_ADDRESS_ON_EEPROM];
  if (currentStatus == STATUS_SELECT)
  {
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
  }
  delay(500);                       // wait for 1/2 seconds
}
