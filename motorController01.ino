
#include "SerialTrace.h"
#include "MenuSelectorSimple.h"


String menu[] =
{
  "Which program v0.2",
  " Watch wind",
  " Mixer",
  ""
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  serialSSI("loop", "-----------", millis());
  MenuSelectorSimple ms = MenuSelectorSimple(menu);
  String s = ms.Select();
  ms.PrintAt(s, 0, 0);
  delay(2000);                       // wait for 2 seconds
}
