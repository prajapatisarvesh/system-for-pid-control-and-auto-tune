//#include <Encoder.h>
//Encoder myEnc(32, 33);
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
bool pc_com = false;
byte at_tune = 16;
const char sep[2] = "\t";
byte lim;
int pos;
char *tk;
//    for (int i = 0; i <= 255; i++) {
//      Serial.print(encoder.read());
//      Serial.print("\t");
//      Serial.print(i + 10);
//      Serial.print("\t");
//      Serial.print(i + 20);
//      Serial.println();
////      delay(50);
//    }
////    delay(50);
//    for (int i = 255; i >= 0; i--) {
//      Serial.print(encoder.read());
//      Serial.print("\t");
//      Serial.print(-i + 10);
//      Serial.print("\t");
//      Serial.print(-i + 20);
//      Serial.println();
////      delay(50);
//    }
//    noInterrupts();
