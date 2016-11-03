#include <OneWire.h>
#include <DallasTemperature.h>

#include "ThingSpeak.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>



ESP8266WiFiMulti WiFiMulti;

int ph_pin = A0; //This is the pin number connected to Po

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
/* 
int R1= 1000;
int Ra=25; //Resistance of powering Pins
int ECPin= A0;
int ECPower =A4; 


float PPMconversion=0.7;
 
 
// *************Compensating for temperature ************************************ //
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
 
 
 
 
// ********************** Cell Constant For Ec Measurements ********************* //
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K=3.08;

*/ 
 
// ************ Temp Probe Related ********************************************* //
#define ONE_WIRE_BUS 4          // Data wire For Temp Probe is plugged into pin 10 on the Arduino
//const int TempProbePossitive =8;  //Temp Probe power connected to pin 9
//const int TempProbeNegative=9;    //Temp Probe Negative connected to pin 8
 
 
 
 
//***************************** END Of Recomended User Inputs *****************************************************************//
 
 
OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.
 
 
float Temperature=10;
float EC=0;
float EC25 =0;
int ppm =0;
int ECRead = 0; 
 
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float buffer=0;
 
/*
  *****************************************************************************************
  **** Visit https://www.thingspeak.com to sign up for a free account and create
  **** a channel.  The video tutorial http://community.thingspeak.com/tutorials/thingspeak-channels/ 
  **** has more information. You need to change this to your channel, and your write API key
  **** IF YOU SHARE YOUR CODE WITH OTHERS, MAKE SURE YOU REMOVE YOUR WRITE API KEY!!
  *****************************************************************************************/
unsigned long myChannelNumber = 178437;
const char * myWriteAPIKey = "XTCN6DCFE8HD2ZTA"; 
 
WiFiClient  client; 
//*********************************Setup - runs Once and sets pins etc ******************************************************//
void setup()
{
  Serial.begin(9600);
  delay(10);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
    
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("mixen-ap-f2", "mixensensagri");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

  ThingSpeak.begin(client);
  
/*  
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
 
  delay(100);// gives sensor time to settle
  sensors.begin();
  delay(100);
  // ** Adding Digital Pin Resistance to [25 ohm] to the static Resistor ********* //
  // Consule Read-Me for Why, or just accept it as true
  R1=(R1+Ra);// Taking into acount Powering Pin Resitance
 
  Serial.println("ElCheapo Arduino EC-PPM measurments");
  Serial.println("By: Michael Ratcliffe  Mike@MichaelRatcliffe.com");
  Serial.println("Free software: you can redistribute it and/or modify it under GNU ");
  Serial.println("");
  Serial.println("Make sure Probe and Temp Sensor are in Solution and solution is well mixed");
  Serial.println("");
  Serial.println("Measurments at 5's Second intervals [Dont read Ec morre than once every 5 seconds]:");
*/ 
 
};
//******************************************* End of Setup **********************************************************************//
 

void loop() {
  // put your main code here, to run repeatedly:

  GetTemp();  

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(ph_pin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;


  int measure = average;
  Serial.print("\n Measure: ");
  Serial.print(measure);

  double voltage = 5 / 1023.0 * measure; //classic digital to voltage conversion
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);

  // PH_step = (voltage@PH7 - voltage@PH4) / (PH7 - PH4)
  // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)

  float PH700V = 2.500;
  //float PH686V = 2.466;
  float PH400V = 2.903;
  
  float PH_Step = (PH700V-PH400V)/(7-4);

  float Po = 7 - ((2.5 - voltage) / PH_Step);


  
  Serial.print("\tPH: ");
  Serial.print(Po, 3);
  Serial.print("\tTemp: ");
  Serial.print(Temperature);
  Serial.print(" *C ");

  float volt = (float)voltage;
  
  ThingSpeak.setField(1,Temperature);
  ThingSpeak.setField(2,Po);
  ThingSpeak.setField(4,volt);
  
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  delay(1000);



/*  
  ECRead++;

if (ECRead > 15){
  GetEC(); 
  PrintReadings();
  ECRead = 0;
}
*/

}

 
 
//************ This Loop Is called From Main Loop************************//

void GetTemp() {

//*********Reading Temperature Of Solution *******************//
sensors.requestTemperatures();// Send the command to get temperatures
Temperature=sensors.getTempCByIndex(0); //Stores Value in Variable
  
}

/*
void GetEC(){

//*********Reading Temperature Of Solution ******************* //
sensors.requestTemperatures();// Send the command to get temperatures
Temperature=sensors.getTempCByIndex(0); //Stores Value in Variable
 
//************Estimates Resistance of Liquid **************** //
digitalWrite(ECPower,HIGH);
raw= analogRead(ECPin);
raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
digitalWrite(ECPower,LOW);
 
 
 
 
//***************** Converts to EC ************************** //
Vdrop= (Vin*raw)/1024.0;
Rc=(Vdrop*R1)/(Vin-Vdrop);
Rc=Rc-Ra; //acounting for Digital Pin Resitance
EC = 1000/(Rc*K);
 
 
//*************Compensating For Temperaure******************** //
EC25  =  EC/ (1+ TemperatureCoef*(Temperature-25.0));
ppm=(EC25)*(PPMconversion*1000);
 
 
;}
//************************** End OF EC Function *************************** //


 
//***This Loop Is called From Main Loop- Prints to serial usefull info *** //
void PrintReadings(){
Serial.print("   Rc: ");
Serial.print(Rc);
Serial.print(" EC: ");
Serial.print(EC25*1000);
Serial.print(" miliSimens  ");
Serial.print(ppm);
Serial.print(" ppm  ");
};

*/
 
