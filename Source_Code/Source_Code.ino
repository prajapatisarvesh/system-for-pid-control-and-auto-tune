#include "define.h"
#include "Cytron.h"
#include "pidautotuner.h"
#include <EEPROM.h>
#define EEPROM_SIZE 126
#include <PID_v1.h>
#include <Encoder.h>
Cytron cytron_4(&Serial1, 001, 0);
double Setpoint, Input, Output;
double kp, ki, kd;
bool go_to_pos = false;
PID myPID(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);
Encoder encoder(34, 35);
//double KP, KI, KD;
double P, I, D;
void setup() {
  //  EEPROM.begin(EEPROM_SIZE);
  //  P = EEPROM.readDouble(3);
  //  I = EEPROM.readDouble(4);
  //  D = EEPROM.readDouble(2);
  Serial.begin(115200);
  Input = encoder.read();
  //  Serial.println(P);
  //  Serial.println(I);
  //  Serial.println(D);
  pinMode(2, OUTPUT);
  Serial1.begin(115200, SERIAL_8N1, 13, 14);
  Serial1.write(128);
  delay(1000);
  //  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-lim, lim);
  myPID.SetSampleTime(10);
  //  inputString.reserve(200);
}

void loop() {
  //  cytron_4.anti_clockwise(150);
  //  Serial.println(encoder.read());
  serialEvent();
}

void get_consts() {
  uint8_t temp = 3;
  uint8_t counter = 0;
  for (int i = 0; i < inputString.length(); i++) {
    if ((char)inputString[i] == ' ' && i > 2) {
      if (counter == 0) {
        kp = inputString.substring(temp, i).toDouble();
        counter++;
      } else if (counter == 1) {
        ki = inputString.substring(temp, i).toDouble();
        counter++;
      } else if (counter == 2) {
        kd = inputString.substring(temp, i).toDouble();
        counter++;
      }
      temp = i;
    }
  }
  //  EEPROM.writeDouble(3, kp);
  //  EEPROM.writeDouble(4, ki);
  //  EEPROM.writeDouble(5, kd);
  //  EEPROM.commit();
  Serial.print(kp);
  Serial.print(" ");
  Serial.print(ki);
  Serial.print(" ");
  Serial.println(kd);
  Serial.flush();
}
void start_at_tune() {
  PIDAutotuner tuner = PIDAutotuner();
  tuner.setTargetInputValue(50);
  tuner.setLoopInterval(2000);
  tuner.setOutputRange(-lim, lim);
  tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);
  tuner.startTuningLoop(micros());
  long microseconds;
  while (!tuner.isFinished())
  {
    long prevMicroseconds = microseconds;
    microseconds = micros();
    double input = encoder.read();
    double output = tuner.tunePID(input, microseconds);
    if (output > 0) cytron_4.clockwise(abs(output));
    else cytron_4.anti_clockwise(abs(output));
    while (micros() - microseconds < 2000) delayMicroseconds(1);
  }
  cytron_4.brake();
  kp = tuner.getKp();
  ki = tuner.getKi();
  kd = tuner.getKd();
  Serial.print("65");
  Serial.print("\t");
  Serial.print(kp);
  Serial.print("\t");
  Serial.print(ki);
  Serial.print("\t");
  Serial.println(kd);
  Serial.flush();

}
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  if (stringComplete) {
    if (inputString.toInt() == 69) {
      Serial.println(inputString);
      Serial.flush();
      digitalWrite(2, 1);
      pc_com = true;
      delay(1500);
    }
    if (inputString.substring(0, 2).toInt() == 68) {
      uint8_t temp1 = 3;
      uint8_t counter1 = 0;
      for (int i = 0; i < inputString.length(); i++) {
        if ((char)inputString[i] == ' ' && i > 2) {
          if (counter1 == 0) {
            lim = inputString.substring(temp1, i).toDouble();
            counter1++;
          }
          temp1 = i;
        }
      }
      Serial.println(inputString.substring(0, 2).toInt());
      Serial.flush();
      myPID.SetOutputLimits(-lim, lim);
      pc_com = false;
      at_tune = 33;
    }
    if (inputString.substring(0, 2).toInt() == 74) {
      uint8_t temp1 = 3;
      uint8_t counter1 = 0;
      for (int i = 0; i < inputString.length(); i++) {
        if ((char)inputString[i] == ' ' && i > 2) {
          if (counter1 == 0) {
            pos = inputString.substring(temp1, i).toInt();
            counter1++;
          }
          temp1 = i;
        }
      }
      Serial.println(pos);
      Setpoint = pos;
      //      Serial.flush();
      //      pc_com = false;
      at_tune = 84;
    }
    if (inputString.substring(0, 2).toInt() == 71) {
      get_consts();
      myPID.SetTunings(kp, ki, kd);
      myPID.SetOutputLimits(-lim, lim);
      pc_com = false;
      at_tune = 44;
    }
    if (inputString.toInt() == 104) {
      encoder.write(0);
    }
    inputString = "";
    stringComplete = false;
    Serial.println("XXXXXXXXXXXXXXXX");
  }
  if (pc_com) {
    Serial.print(encoder.read());
    Serial.print("\t");
    Serial.print(encoder.read());
    Serial.print("\t");
    Serial.println(encoder.read());
    Serial.flush();
    delay(50);
  }
  if (at_tune == 33) {
    start_at_tune();
    myPID.SetTunings(kp, ki, kd);
    at_tune = 44;
    delay(50);
  }
  if (at_tune == 44) {
    Serial.print(encoder.read());
    Serial.print("\t");
    Serial.print(encoder.read());
    Serial.print("\t");
    Serial.println(encoder.read());
    Serial.flush();
    delay(1);
  }
  if (at_tune == 84) {

    Input = encoder.read();
    //    PID myPID(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);
    //    myPID.SetMode(AUTOMATIC);
    //    myPID.SetOutputLimits(-lim, lim);
    //    myPID.SetTunings(0.27, 0.01, 1.96);
    //    myPID.SetSampleTime(10);
    myPID.Compute();
    Serial.print(myPID.GetKp());
    Serial.print(" ");
    Serial.print(myPID.GetKi());
    Serial.print(" ");
    Serial.println(myPID.GetKd());
    myPID.SetTunings(kp * 4, ki + 4, kd * 0.007);
    if (Output > 0)
    {
      //      Serial.println("herer");
      cytron_4.clockwise(abs(Output));
    }
    else
    {
      //      Serial.println("got it");
      cytron_4.anti_clockwise(abs(Output));
    }
    //    if (Input - Setpoint > -10 || Input - Setpoint < 10)
    //    {
    //      myPID.SetTunings(kp * 2.7, ki + 3.0, kd * 0.007);
    //    }
    //    else if (Input - Setpoint > 30 || Input - Setpoint < 30)
    //    {
    //      myPID.SetTunings(kp * 2.05, ki + 1, kd * 0.001736);
    //    }
    //      Serial.flush();
    delay(1);
    //    Serial.print(encoder.read());
    //    Serial.print("\t");
    //    Serial.print(encoder.read());
    //    Serial.print("\t");
    //    Serial.println(encoder.read());
    Serial.flush();
    //    at_tune = 44;
    //    0.39  0.00  11.52

  }
  if (!pc_com || !at_tune) {
    //    Serial.println(encoder.read());
    //    cytron_4.brake();
    Serial.flush();
  }
}
