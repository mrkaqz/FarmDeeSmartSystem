int ph_pin = A2; //This is the pin number connected to Po

void setup() {
  Serial.begin(9600);
}

void loop() {
  int measure = analogRead(ph_pin);
  Serial.print("Measure: ");
  Serial.print(measure);

  double voltage = 5 / 1024.0 * measure; //classic digital to voltage conversion
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);

  // PH_step = (voltage@PH7 - voltage@PH4) / (PH7 - PH4)
  // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)

  float PH700V = 2.500;
  float PH686V = 2.466;
  float PH400V = 2.935;
  
  float PH_Step = (PH700V-PH400V)/(7-4);

  float Po = 7 - ((2.5 - voltage) / PH_Step);
  Serial.print("\tPH: ");
  Serial.print(Po, 3);

  Serial.println("");
  delay(2000);
}
