int MotorControl[4] = {4,5,6,7};

void setup() {
  // put your setup code here, to run once:
  
  pinMode(MotorControl[0], OUTPUT);
  pinMode(MotorControl[1], OUTPUT);
  pinMode(MotorControl[2], OUTPUT);
  pinMode(MotorControl[3], OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;
  for(i = 0;i<=4;i++){
    digitalWrite(MotorControl[i],HIGH);
    delay(1000);
    //digitalWrite(MotorControl[i],LOW);
    //delay(1000);
}
}
