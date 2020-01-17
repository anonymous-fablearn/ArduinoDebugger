#include <ArduinoDebugger.h>
//Code is a modified version from Adafruit's Temperature tutorial
//https://learn.adafruit.com/tmp36-temperature-sensor/using-a-temp-sensor
int sensorPin = A0;
int sensorReading = 0;//Value read from sensor
float voltage = 0;//Convert sensor's reading to voltage
float tempC = 0;//Celsius
float tempF = 0;//Fahrenheit
int normalLed = 4;
int hotLed = 5;
int coldLed = 6;

ArduinoDebugger debugger(true);//true - 8 Bit AVR, false - 32 bit chip
void setup() {
  Serial.begin(9600);
  while(!Serial){}//Wait until Serial is ready
	pinMode(normalLed, OUTPUT);
	pinMode(hotLed, OUTPUT);
	pinMode(coldLed, OUTPUT);
	debugger.addWatch(&sensorReading, INT, "sensorReading");
	debugger.addWatch(&voltage, FLOAT, "voltage");
	debugger.addWatch(&tempC, FLOAT, "temp_C");
	debugger.addWatch(&tempF, FLOAT, "temp_F");
}

void loop() {
	sensorReading = analogRead(sensorPin);
	voltage = (sensorReading * 5.0)/1024.0;//Convert to reading volts 
  tempC = (voltage - 0.5) * 100 ;//Convert to Celsius
  tempF = (tempC * 9.0 / 5.0) + 32.0;//Convert to Fahrenheit
	if(tempF > 80)
	{//over 80 degrees, it's HOT!
		digitalWrite(normalLed, LOW);
		digitalWrite(hotLed, HIGH);
		digitalWrite(coldLed, LOW);
	}
	else if(tempF < 60)
	{//Under 60 degrees, it's COLD!
		digitalWrite(normalLed, LOW);
		digitalWrite(hotLed, LOW);
		digitalWrite(coldLed, HIGH);
	}
	else
	{//Normal temperature
		digitalWrite(normalLed, HIGH);
		digitalWrite(hotLed, LOW);
		digitalWrite(coldLed, LOW);
	}
  debugger.breakpoint();//Debug program
	//debugger.displayVariables();//View all variable values
  delay(1000);
}