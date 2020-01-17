#include <ArduinoDebugger.h>

int pin_position = A1;//Top potentiometer
int pin_brightness = A2;//Bottom potentiometer
int led_position = 0;//Current LED on
int led_brightness = 0;//Current brightness level
int leds[] = {3, 5, 6, 9};//Pins for LEDs (must be PWM)
int leds_length = 4;//Number of leds
ArduinoDebugger debugger(true);//true - 8 Bit AVR, false - 32 bit chip
void setup() {
  Serial.begin(9600);
  while(!Serial){}//Wait until Serial is ready
	debugger.addWatch(&led_position, INT, "led_position");
	debugger.addWatch(&led_brightness, INT, "led_brightness");
	//Set each LED as output
	for(int i = 0; i < leds_length; i++)
	{
		pinMode(leds[i], OUTPUT);
	}
}

void loop() {
	//update potentiometer values
	led_position = analogRead(pin_position);
	led_brightness = analogRead(pin_brightness);
	
	debugger.breakpoint();//update pot_brightness & pot_position
	//debugger.displayVariables();//Only view variable information
	
	//Convert the potentiometer's value to an array index
	int map_position = map(led_position, 0, 1023, 0, leds_length - 1);
	
	//Convert the potentiometer's value to a brighness level
	int map_brightness = map(led_brightness, 0, 1023, 0, 255);
	
	//Update each LED's brightness
	for(int i = 0; i < leds_length; i++)
	{
		digitalWrite(leds[i], LOW);//Turn of each LED
		if(i == map_position)
		{//Set the chosen LED to chosen brightness level
			analogWrite(leds[i], map_brightness);
		}
	}
}