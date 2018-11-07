#if true
void serialSI(String s, int i) { Serial.print(s);Serial.print(" ");Serial.println(i, DEC); }
void serialSSI(String s1, String s2, int i) { Serial.print(s1);Serial.print(" ");Serial.print(s2);Serial.print(" ");Serial.println(i, DEC); }
void serialSSS(String s1, String s2, String s3) { Serial.print(s1);Serial.print(" ");Serial.print(s2);Serial.print(" ");Serial.println(s3); }
#else
#define serialSI(a,b)
#define serialSSI(a,b,c)
#define serialSSS(a,b,c)
#endif
