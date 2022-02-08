void pid_init() {
  //  EEPROM.begin(EEPROM_SIZE);
  //  P = EEPROM.readDouble(3);
  //  I = EEPROM.readDouble(4);
  //  D = EEPROM.readDouble(2);
  Serial.begin(115200);
  Input = encoder.read();
  //  Serial.println(P);
  //  Serial.println(I);
  //  Serial.println(D);
  Serial1.begin(115200, SERIAL_8N1, 13, 14);
  Serial1.write(128);
  delay(1000);
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-lim, lim);
  myPID.SetSampleTime(10);
  //  inputString.reserve(200);
}
void pid_position() {
  Input = encoder.read();
  myPID.Compute();
  //  Serial.print(myPID.GetKp());
  //  Serial.print(" ");
  //  Serial.print(myPID.GetKi());
  //  Serial.print(" ");
  //  Serial.println(myPID.GetKd());
  if (Output > 0)
  {
    cytron_4.clockwise(abs(Output));
  }
  else
  {
    cytron_4.anti_clockwise(abs(Output));
  }
  if (Input - Setpoint > 10 || Input - Setpoint < 10)
  {
    myPID.SetTunings(kp * 2.64, ki + 2.9, kd * 0.006178);
  }
  else if (Input - Setpoint > 30 || Input - Setpoint < 30)
  {
    myPID.SetTunings(kp * 2.05, ki + 1, kd * 0.001736);
  }
  delay(1);
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
  myPID.SetTunings(kp, ki, kd);
  myPID.SetOutputLimits(-lim, lim);
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
  myPID.SetOutputLimits(-lim, lim);
  PIDAutotuner tuner = PIDAutotuner();
  tuner.setTargetInputValue(1000);
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
