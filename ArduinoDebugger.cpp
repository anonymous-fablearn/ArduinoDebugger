#include <ArduinoDebugger.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
//Todo:
//Error messages are repeated, only calling method should display error messages
//Create menu bar

//Create a monitor("point name") method which can display variables in a terminal
// with a constant refresh
ArduinoDebugger::ArduinoDebugger(bool isAvr, bool usingTerminal)
{
	_isAvr = isAvr;
	_usingTerminal = usingTerminal;
}

/**********************************************************
* @brief Set a breakpoint with no name
*
*  The breakpoint method will provide a menu to the user
*  so that they can view & update the systems:
*			-Pins
*			-Watched Variables
*	 This method also saves the value of the millis() when the method was called.
*  Upon exiting the breakpoint method, millis() will be returned to its previous value (as long as
*  millis() wasn't updated by the user).
***********************************************************/
void ArduinoDebugger::breakpoint()
{
	breakpoint("");
}
/**********************************************************
* @brief Set a breakpoint with a label to help 
*				 define which breakpoint in the program is being viewed.
*
*  The breakpoint method will provide a menu to the user
*  so that they can view & update the systems:
*			-Pins
*			-Watched Variables
*			-Millis() Timer 
*	 
*	@note This method also saves the value of the millis() when the method was called.
*  Upon exiting the breakpoint method, millis() will be returned to its previous value (as long as
*  millis() wasn't updated by the user).
*	@param name A string for labeling the breakpoint.  
***********************************************************/
void ArduinoDebugger::breakpoint(char name[])
{
	clearScreen();//Clear Screen
	byte selection;
	do{
		//Debugger Menu
		Serial.print(F("\tBreakpoint - "));
		Serial.println(name);
		Serial.println(F("1. Hardware Pins \n2. Variables"));
		Serial.println(F("What component would you like to view and/or update?"));
		selection = getSelection();
		switch(selection)
		{
			case 1://1. Display Pin States
				clearScreen();
				updatePins();
				clearScreen();
				break;
				
			case 2://2. Display Variable Values
				clearScreen();
				updateVariables();
				clearScreen();
				break;
				
			case 3://Exit debugging 
			case 255://Accepts Q 
			  selection = 3;
				Serial.println(F("\nContinuing Program"));
				break;
				
			default:
				clearScreen();
				Serial.println(F("Invalid Selection"));
		}
	}while(selection != 3);
}

/**********************************************************
* @brief Add a variable to our watch list.
*
*  A watched variable can be viewed or updated via the breakpoint method.  
*   
*	@param ptr The memory location of a variable being watched
*	@param type This is an Enum representing the variable's data type
*	@param name	The name of variable being watched
***********************************************************/
void ArduinoDebugger::add(void *ptr, Type type, char name[])
{
	if(strlen(name) <= 15)
	{//Verify name will fit inside a struct's name (char[16])
		//Todo verify variable has not already been added to the variable
		//watch to prevent variables from being added over n' over with each loop
		bool exists = false;
		for(byte i = 0; i < top_var_watch; i++)
		{
			Variable check = var_watch[i];
			if(strcmp(check.name, name) == 0)
			{//Variable name found
				exists = true;
			}
		}
		if(!exists)
		{//Verify the variable has not already been added to the watch list
			Variable temp = {ptr, type};
			memcpy(temp.name, name, strlen(name) + 1);
			if(top_var_watch < sizeof(var_watch)/sizeof(var_watch[0]))
			{//Check that there is room for an additional variable
				var_watch[top_var_watch] = temp;
				top_var_watch += 1;
			}
			else
			{
				clearScreen();
				drawStars();
				Serial.print(F("Can't add "));
				Serial.println(name);
				Serial.println(F("Too many variables in watch"));
				drawStars();
			}
		}
	}
	else
	{
		Serial.print(name);
		Serial.println(F("-Name must be <=15 characters"));
	}
}

void ArduinoDebugger::remove(char var_name[])
{
	bool found = false;
	for(byte i = 0; i < top_var_watch; i++)
	{
		Variable temp = var_watch[i];
		if(!found)
		{
			if(strcmp(temp.name, var_name) == 0)
			{//Variable name found
				found = true;
			}
		}
		else
		{
			var_watch[i - 1] = temp;//Replace the previous entry
		}
	}
	if(!found)
	{
		Serial.print(F("Unable to remove "));
		Serial.println(var_name);
		Serial.println(F("It was not listed in the variable watch"));
	}
	else
	{
		top_var_watch--;
	}
}



/**********************************************************
* @brief Display current Digital/Analog value of all pins.
*
*  displayPins() will display the current state of all Digital &
*	 Analog Pins.  The method also allows for the state of a Digital Pin 
*	 to be updated (HIGH/LOW).  
*
* @note Currently, this method does not supported viewing/updating PWM values.
*  
*	 
***********************************************************/
void ArduinoDebugger::displayPins()
{
	drawStars();
	Serial.println(F("\t\tDigital Pin State\n\tPin Number : State(LOW/HIGH/HIGH(Power In)"));
	drawStars();
	for(int i = 0; i < NUM_DIGITAL_PINS; i++)
	{
		Serial.print(i);
		Serial.print(F(" : "));
		bool power_out = false;
		volatile void* out;
		if(_isAvr)
		{//Use 8 bit values for AVR chips
			uint8_t bit_val = digitalPinToBitMask(i);
			out = portOutputRegister(digitalPinToPort(i));
			bit_val &= *static_cast<volatile uint8_t*>(out);
			if(bit_val > 0)
			{
				power_out = true;
			}
		}
		else
		{//Non AVR chips (SAMD or NRF [:Bit]) use 32 bit
			uint32_t bit_val = digitalPinToBitMask(i);
			out = portOutputRegister(digitalPinToPort(i));
			bit_val &= *static_cast<volatile uint32_t*>(out);
			if(bit_val > 0)
			{
				power_out = true;
			}
		}
		
		if(power_out)
		{//Pin set to HIGH -> Providing Power
			Serial.println(F("HIGH"));
		}
		else if(digitalRead(i) == HIGH)
		{
			Serial.println(F("HIGH(Power In)"));
		}
		else
		{
			Serial.println(F("LOW"));
		}		
	}
	
	drawStars();
	Serial.println(F("\t\tAnalog Pin State\n\tPin Number : Analog Value"));
	drawStars();
	for(int i = A0; i < NUM_ANALOG_INPUTS + A0; i++)
	{
		Serial.print(F("A"));
		Serial.print(i-A0);
		Serial.print(F(" : "));
		Serial.println(analogRead(i));
	}
}

//Incomplete
//Update to work with byte arrays & determine their size!
void ArduinoDebugger::displayPins(byte digital_pins[], byte analog_pins[])
{
	Serial.println(F("\t\tDigital Pin State\n\tPin Number : State(LOW/HIGH/HIGH(Power In)"));
	drawStars();
	for(int i = 0; i < NUM_DIGITAL_PINS; i++)
	{
		Serial.print(i);
		Serial.println(F(" : "));
		bool power_out = false;
		volatile void* out;
		if(_isAvr)
		{//Use 8 bit values for AVR chips
			uint8_t bit_val = digitalPinToBitMask(i);
			out = portOutputRegister(digitalPinToPort(i));
			bit_val &= *static_cast<volatile uint8_t*>(out);
			if(bit_val > 0)
			{
				power_out = true;
			}
		}
		else
		{//Non AVR chips (SAMD or NRF [:Bit]) use 32 bit
			uint32_t bit_val = digitalPinToBitMask(i);
			out = portOutputRegister(digitalPinToPort(i));
			bit_val &= *static_cast<volatile uint32_t*>(out);
			if(bit_val > 0)
			{
				power_out = true;
			}
		}
		
		if(power_out)
		{//Pin set to HIGH
			Serial.println(F("HIGH"));
		}
		else if(digitalRead(i) == HIGH)
		{
			Serial.println(F("HIGH(Power In)"));
		}
		else
		{
			Serial.println(F("LOW"));
		}		
	}
	
	drawStars();
	Serial.println(F("\t\tAnalog Pin State\n\tPin Number : Analog Value"));
	drawStars();
	for(int i = A0; i < NUM_ANALOG_INPUTS + A0; i++)
	{
		Serial.print(F("A"));
		Serial.print(i-A0);
		Serial.print(F(" : "));
		Serial.println(analogRead(i));
	}
}


/**********************************************************
* @brief Prompt the user for the value to update the pin to.
*
*  This method will prompt the user for the new digital value (HIGH/LOW)
*  to update the pin to.  
*
*  @note If an invalid value is entered, no change will be made to the pin.
*  
*	 @param pin The chosen pin to update
***********************************************************/
void ArduinoDebugger::updatePins()
{
	byte selection = 'Q';
	displayPins();
	Serial.println(F("Enter the Pin Number for the Digital Pin to be updated."));
	byte pin = getSelection();
	while(pin != 255)
	{
		if(pin < NUM_DIGITAL_PINS && pin != 254)
		{
			Serial.print(F("Set "));
			Serial.print(pin);
			Serial.print(F(" to (H)IGH or (L)OW: "));
			while(Serial.available() == 0){}
			byte state = Serial.peek();
			if(!_usingTerminal)
			{
				while(Serial.available() != 0)
				{
					Serial.print((char)Serial.read());//Echo value back to Serial Monitor
				}
			}
			clearScreen();
			if(state == 'H' || state == 'h')
			{
				digitalWrite(pin, HIGH);
			}
			else if(state == 'L' || state == 'l')
			{
				digitalWrite(pin, LOW);
			}
			else
			{
				Serial.println(F("Invalid Value"));
			}
		}
		else
		{
			Serial.println(F("Invalid Selection"));
		}
		clearBuffer();
		displayPins();
		pin = getSelection();	
	}
	drawStars();
}

/**********************************************************
* @brief Display all variables being watched with their current values.
*
*  This method will display each of the variables (w/ their values) 
*  that have been added to the watch list (via add() method).  This
*	 method will then allow the user to update any of the watched variables.
*  
*	 
***********************************************************/
void ArduinoDebugger::displayVariables()
{
	Serial.println(F("\t\tVariable Watch"));
	Serial.println(F("[index] variable_name (type) : value"));
	drawStars();
	for(int i =0; i < top_var_watch; i++)
	{
		 Variable temp = var_watch[i];
		 Serial.print(F("["));
		 Serial.print(i);
		 Serial.print(F("] "));
		 switch(temp.type)
		 {		
				case BYTE:
				  Serial.print(temp.name);
					Serial.print(F(" (byte): "));
					Serial.println(*((byte*)temp.ptr));
					break;
					
				case Type::INT:
					Serial.print(temp.name);
					Serial.print(F(" (int): "));
					Serial.println(*((int*)temp.ptr));
					break;
			
				case Type::LONG:
					Serial.print(temp.name);
					Serial.print(F(" (long): "));
					Serial.println(*((long*)temp.ptr));
					break;
					
				case Type::FLOAT:
					Serial.print(temp.name);
					Serial.println(F(" (float): Set usingFloats to true in initalize()"));
					break;
					
				case Type::CHAR:
					Serial.print(temp.name);
					Serial.print(F(" (char): "));
					Serial.println(*((char*)temp.ptr));
					break;
						
				case Type::BOOL:
					Serial.print(temp.name);
					if(*(bool*)(temp.ptr))
					{
					Serial.print(F(" (bool): true"));
					}
					else
					{
						Serial.print(F(" (false): true"));
					}
					break;
					
				
				case Type::BYTE_ARRAY:
				case Type::INT_ARRAY:
				case Type::LONG_ARRAY:
				case Type::CHAR_ARRAY:
				case Type::BOOL_ARRAY:
					displayArray(temp);
					break;
		 }
	}
	drawStars();
}

/**********************************************************
* @brief Prompt the user for the value to update the chosen variable to.
*
*  This method will prompt the user for a new value to use for a chosen
*  variable and validate the new value. and 
*
*  @note The variable is only updated if a valid value is entered.
*  Invalid values will cause error messages to be displayed.
*  
*	 @param index The index of the variable to update
***********************************************************/
void ArduinoDebugger::updateVariables()
{
	displayVariables();
	Serial.println(F("Enter the index of the variable to be updated.")); 
	byte index = getSelection();
	while(index != 255)
	{
		if(index < top_var_watch && index != 254)
		{//Verify valid index was chosen
			Variable temp = var_watch[index];
			bool valid = true;
			long retrieved_val = 0;
			if(temp.type == Type::BYTE_ARRAY || 
				 temp.type == Type::INT_ARRAY ||
				 temp.type == Type::LONG_ARRAY ||
				 temp.type == Type::CHAR_ARRAY ||
				 temp.type == Type::BOOL_ARRAY)
			{
				updateArray(temp);
			}
			else
			{
				Serial.print(F("New value for "));
				Serial.print(temp.name);
				switch(temp.type)
				{					
					case Type::BYTE:
						Serial.print("(byte): ");
						retrieved_val = getNumber(valid, Type::BYTE);
						if(valid)
						{
							clearScreen();
							*(byte*)(temp.ptr) = (byte) retrieved_val;
						}
						break;
					
					case Type::INT:
						Serial.print("(int): ");
						retrieved_val = getNumber(valid, Type::INT);
						if(valid)
						{
							clearScreen();
							*(int*)(temp.ptr) = (int) retrieved_val;
						}
						break;
						
					case Type::LONG:
						Serial.print("(long): ");
						retrieved_val = getNumber(valid, Type::LONG);
						if(valid)
						{
							clearScreen();
							*(long*)(temp.ptr) = (long) retrieved_val;
						}
						break;
					
					case Type::Float:
						Serial.println(F(" (float): Set usingFloats to true in initalize()"));
						break;
						
					case Type::CHAR:
						*(char*)(temp.ptr) = getChar();
						break;
						
					case Type::BOOL: 
						bool temp_bool = getBool(valid);
						if(valid)
						{
							*(bool*)(temp.ptr) = temp_bool;
						}
						break;
				}
			}
		}
		else
		{
			Serial.println(F("Invalid Selection"));
		}
		displayVariables();
		index = getSelection();
	}
}

void ArduinoDebugger::printName(char name[])
{
	//Expected format for array variable name:
	//     varName_SIZE
	// Ex: pins_20
	char trim_name[16];
	char* ptr = strchr(name, '_');
	int size = ptr - name;
	strncpy(trim_name, name, size);
	trim_name[size] = 0;
	Serial.print(trim_name);
}

byte ArduinoDebugger::getSize(char name[])
{
	//Expected format for array variable name:
	//     varName_SIZE
	// Ex: pins_20
	char* end = strchr(name, '_');//The last underscore is the end of the variable name
	byte size = 0;
	if(end != NULL)
	{
		size=atoi(end + 1);//Shorten to the actual variable name
	}
	return size;
}

void ArduinoDebugger::displayArray(Variable var)
{
	printName(var.name);
	switch(var.type)
		{
			case Type::BYTE_ARRAY:
				Serial.println(F(" byte[]:"));
				break;
				
			case Type::INT_ARRAY:
				Serial.println(F(" int[]:"));
				break;
				
			case Type::LONG_ARRAY:
				Serial.println(F(" long[]:"));
				break;
				
			case Type::CHAR_ARRAY:
				Serial.println(F(" char[]:"));
				break;
				
			case Type::BOOL_ARRAY:
			  Serial.println(F(" bool[]:"));
				break;
		}
	byte size = getSize(var.name);
	for(byte i = 0; i < size; i++ )
	{
		Serial.print(F("\t"));
		printName(var.name);
		Serial.print(F("["));
		Serial.print(i);
		Serial.print(F("]: "));
		switch(var.type)
		{
			case Type::BYTE_ARRAY:
				Serial.println(((byte*)var.ptr)[i]);
				break;
				
			case Type::INT_ARRAY:
				Serial.println(((int*)var.ptr)[i]);
				break;
				
			case Type::LONG_ARRAY:
				Serial.println(((long*)var.ptr)[i]);
				break;
				
			case Type::CHAR_ARRAY:
				Serial.println(((char*)var.ptr)[i]);
				break;
				
			case Type::BOOL_ARRAY:
			  if(((bool*)var.ptr)[i])
				{
					Serial.println(F("true"));
				}
				else
				{
					Serial.println(F("false"));
				}
				break;
		}
	}
}

void ArduinoDebugger::updateArray(Variable var)
{
	clearScreen();
	displayArray(var);
	Serial.println(F("Enter array index to be updated."));
	byte index = getSelection();
	while(index != 255 )
	{
		byte size = getSize(var.name);
		if(index < size && index != 254)
		{
			bool valid = true;
			long retrieved_val = 0;
			Serial.print(F("New value for ["));
			Serial.print(index);
			Serial.println(F("]: "));
			switch(var.type)
			{
				case Type::BYTE_ARRAY:
					retrieved_val = getNumber(valid, Type::BYTE);
					if(valid)
					{
						((byte*)var.ptr)[index] = (byte)retrieved_val;
					}
					break;
					
				case Type::INT_ARRAY:
					retrieved_val = getNumber(valid, Type::INT);
					if(valid)
					{
						((int*)var.ptr)[index] = (int)retrieved_val;
					}
					break;
					
				case Type::LONG_ARRAY:
					retrieved_val = getNumber(valid, Type::LONG);
					if(valid)
					{
						((long*)var.ptr)[index] = (long)retrieved_val;
					}
					break;
					
				case Type::CHAR_ARRAY:
						((char*)var.ptr)[index]  = getChar();
						break;
					
				case Type::BOOL_ARRAY: 
					bool temp_bool = getBool(valid);
					if(valid)
					{
						((bool*)var.ptr)[index] = temp_bool;
					}
					break;
			}
		}
		else
		{
			Serial.println(F("Invalid Selection"));
		}
		clearScreen();
		displayArray(var);
		Serial.println(F("Enter array index to be updated."));
	  index = getSelection();
	}
	
}


byte ArduinoDebugger::getSelection()
{
	byte selection = 0;
	Serial.print(F("Selection (Q to quit): "));
	while(Serial.available() == 0){}
	selection = Serial.peek();
	if(selection == 'Q' || selection == 'q')
	{
		selection = 255;//Exit menu
	}
	else
	{
		bool valid = true;
		selection = getNumber(valid, BYTE);
		if(!valid)
		{
			selection = 254;//Error occurred
			Serial.println(F("Invalid Selection"));
		}
	}
	clearBuffer();
	return selection;
}

/**********************************************************
* @brief Retrieve a number from the user and ensure it is both valid
*        and fits within a set number of bytes.
*
*  This method will read an ASCII number from the Serial port using Arduino'saves
*  Serial Library.  This method checks for valid values on 8-Bit & 32-Bit microcontrollers
*	 8-Bit: Int (2 Bytes), Long (4 Bytes)
*	 32-Bit: Int (4 Bytes), Long (8 Bytes)
*
*
*  @note If the converted number causes overflow or does not fit within
*  the set number of bytes (parameter), this method will set the parameter
*  valid to false.
*  
*	 @param valid - a boolean (passed by reference)
*	 @param type - The type of number being used
***********************************************************/
long ArduinoDebugger::getNumber(bool &valid, Type type)
{
	valid = true;//Assume value is valid
	bool outRange = false;//Determine if the number is too large/small
	char* eptr;//Points to the first character that COULD NOT be converted, may be null if all values could be converted
  char input[12];
  while(Serial.available() == 0) {}//Wait for value
  int num = Serial.readBytes(input, 11);
  input[num] = 0;//Null terminate
	if(!_usingTerminal)
	{
		Serial.println(input);//echo input to screen
	}
  long result = strtol(input, &eptr, 10);//Convert user input to long
  if(!(*eptr == 0 || *eptr == 13 || *eptr == 10))
  {//Check if a non-numeric character was entered
    Serial.println(F("Not a number"));
		valid = false;
  }
  else if(errno == ERANGE)
  {//Check if the value was too large for a long

    outRange = true;
	}
	else if(type != LONG){
		long check = 0;
		switch(type)
		{//Verify the user input fits in the destination size
			case Type::BYTE:
				check = (byte)result; 
				break;
				
			case Type::INT:
				check = (int)result;
				break;
				
		}
		if(check != result)
		{
			outRange = true;
		}
	}
	if(outRange)
	{
		Serial.println(F("Value Out of Range"));
		valid = false;
	}
	return result;
}

char ArduinoDebugger::getChar()
{
	Serial.print(F("(char): "));
	while(Serial.available() == 0){}
	char temp_char = Serial.read();
	if(!_usingTerminal)
	{
		Serial.println(temp_char);
	}
	clearBuffer();
	clearScreen();
	return temp_char;
}


bool ArduinoDebugger::getBool(bool &valid)
{
	bool value = true;
	valid = true;
	Serial.print("(bool): ");
	while(Serial.available() == 0){}
	byte bool_char = Serial.peek();
	
	if(!_usingTerminal)
	{
		while(Serial.available() != 0)
		{
			Serial.print((char)Serial.read());//Echo value back to Serial Monitor
		}
	}
	
	if(bool_char == 'F' || bool_char == 'f')
	{
		value = false;
	}
	else if(bool_char != 'T' && bool_char != 't')
	{
		valid = false;
		Serial.println(F("Value must be (t)rue or (f)alse"));
	}
	clearBuffer();
	return value;
}
/**************************************************************************
* 
*  @brief Pause the currently running Arduino program
*	
*	This method will pause the arduino program and wait until the user
*			enters a value via the Serial port to continue
*	@note This method requires the Serial port to have a connection in order to
*			work correctly.
***/    
void ArduinoDebugger::pause()
{
	if(Serial)
	{//Verify the Serial port is accessible
		Serial.println(F("Press ENTER to continue"));
		while(Serial.available() == 0){} //Wait until input is provided on the Serial port
		clearBuffer();
	}
}


/**************************************************************************
* 
*  @brief Clear the Serial Buffer
*	
*	This method will remove any characters within the Serial Buffer
***/
void ArduinoDebugger::clearBuffer()
{ int flush = timedPeek();
	while(flush != -1)
	{//remove any remaining values from the Serial buffer
		Serial.read();
		flush = timedPeek();
	}
}

/**************************************************************************
* 
*  @brief Clear the screen on Serial Monitor
*	
*	This method will "clear" the screen by feeding a series of newlines to move the
* location in the Serial Monitor to be further down.  
***/
void ArduinoDebugger::clearScreen()
{
	if(_usingTerminal)
	{
		Serial.write(27);
		Serial.print(F("[2J"));// clear screen command
		Serial.write(27);
		Serial.print(F("[H"));
	}
	else
	{
		for(int i = 0; i < 15; i++)
		{
			Serial.println();
		}
	}
}

/***********************************************************
* 
*  Print a series of asterisks on the Serial monitor.
*	
***/
void ArduinoDebugger::drawStars()
{
	for(int i = 0; i < 50; i++)
	{
		Serial.print('*');
	}
	Serial.println();
}

static int ArduinoDebugger::timedPeek()
{
  int c;
  unsigned long startMillis = millis();
  do {
    c = Serial.peek();
    if (c >= 0) return c;
  } while(millis() - startMillis < 1000);//default timeout
  return -1;     // -1 indicates timeout
}