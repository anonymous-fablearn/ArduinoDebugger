#include <ArduinoDebuggerF.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


/**********************************************************
* @brief Display all variables being watched with their current values.
*
*  This method will display each of the variables (w/ their values) 
*  that have been added to the watch list (via addWatch() method).  This
*	 method will then allow the user to update any of the watched variables.
*  
*	 
***********************************************************/
void ArduinoDebuggerF::displayVariables()
{
	Serial.println(F("\t\tVariable Watch\n\t[index] variable_name (type) : value"));
	drawStars();
	for(int i =0; i < top_var_watch; i++)
	{
		 Variable temp = var_watch[i];
		 Serial.print(F("["));
		 Serial.print(i);
		 Serial.print(F("] "));
		 switch(temp.type)
		 {		
				case Type::BYTE:
				  Serial.print(temp.name);
					Serial.print(F(" (byte): "));
					Serial.println(*((byte*)temp.ptr));
					break;
				
				case Type::BYTE_ARRAY:
					printName(temp.name);
					Serial.print(F(" (byte["));
					Serial.print(getSize(temp.name));
					Serial.println(F("] : Select to view"));
					break;
					
				case Type::INT:
					Serial.print(temp.name);
					Serial.print(F(" (int): "));
					Serial.println(*((int*)temp.ptr));
					break;
				
				case Type::INT_ARRAY:
					printName(temp.name);
					Serial.print(F(" (int["));
					Serial.print(getSize(temp.name));
					Serial.println(F("] : Select to view"));
					break;				
					
				case Type::LONG:
					Serial.print(temp.name);
					Serial.print(F(" (long): "));
					Serial.println(*((long*)temp.ptr));
					break;
					 
				case Type::LONG_ARRAY:
					printName(temp.name);
					Serial.print(F(" (long["));
					Serial.print(getSize(temp.name));
					Serial.println(F("] : Select to view"));
					break;
					
				case Type::FLOAT:
					Serial.print(temp.name);
					Serial.print(F(" (float): "));
					Serial.println(*((float*)temp.ptr));
					break;
					
				case Type::FLOAT_ARRAY:
					printName(temp.name);
					Serial.print(F(" (float["));
					Serial.print(getSize(temp.name));
					Serial.println(F("] : Select to view"));
					break;
								
				case Type::CHAR:
					Serial.print(temp.name);
					Serial.print(F(" (char): "));
					Serial.println(*((char*)temp.ptr));
					break;
					
				case Type::CHAR_ARRAY:
					printName(temp.name);
					Serial.print(F(" (char["));
					Serial.print(getSize(temp.name));
					Serial.println(F("] : Select to view"));
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
					
				case Type::BOOL_ARRAY:
					printName(temp.name);
					Serial.print(F(" (bool["));
					Serial.print(getSize(temp.name));
					Serial.println(F("] : Select to view"));
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
void ArduinoDebuggerF::updateVariables()
{
	displayVariables();
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
				 temp.type == Type::FLOAT_ARRAY ||
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
						
					case Type::FLOAT: 
							*(float*)(temp.ptr) = getFloat();
							clearScreen();
						break;

					case Type::CHAR:
						*(char*)(temp.ptr) = getChar();
						break;
						
					case Type::BOOL: 
						bool temp_bool = getBool(valid);
						if(valid)
						{
							clearScreen();
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

void ArduinoDebuggerF::displayArray(Variable var)
{
	drawStars();
	printName(var.name);
	Serial.println(F(" [index] : value "));
	byte size = getSize(var.name);
	for(byte i = 0; i < size; i++ )
	{
		Serial.print(F("["));
		Serial.print(i);
		Serial.println(F("]: "));
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
			
			case Type::FLOAT_ARRAY:
				Serial.println(((float*)var.ptr)[i]);
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

void ArduinoDebuggerF::updateArray(Variable var)
{
	displayArray(var);
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
						clearScreen();
						((byte*)var.ptr)[index] = (byte)retrieved_val;
					}
					break;
					
				case Type::INT_ARRAY:
					retrieved_val = getNumber(valid, Type::INT);
					if(valid)
					{
						clearScreen();
						((int*)var.ptr)[index] = (int)retrieved_val;
					}
					break;
					
				case Type::LONG_ARRAY:
					retrieved_val = getNumber(valid, Type::LONG);
					if(valid)
					{
						clearScreen();
						((long*)var.ptr)[index] = (long)retrieved_val;
					}
					break;
				
				case Type::FLOAT_ARRAY:
					((float*)var.ptr)[index] = getFloat();
					clearScreen();
					break;
				
				case Type::CHAR_ARRAY:
						((char*)var.ptr)[index]  = getChar();
						break;
					
				case Type::BOOL_ARRAY: 
					bool temp_bool = getBool(valid);
					if(valid)
					{
						clearScreen();
						((bool*)var.ptr)[index] = temp_bool;
					}
					break;
			}
		}
		else
		{
			Serial.println(F("Invalid Selection"));
		}
		displayArray(var);
	  index = getSelection();
	}
	
}				
				
				

float ArduinoDebuggerF::getFloat()
{
	char input[12];
  while(Serial.available() == 0) {}//Wait for value
  int num = Serial.readBytes(input, 11);
  input[num] = 0;//Null terminate
	if(!_usingTerminal)
	{
		Serial.println(input);//echo input to screen
	}
	float result = atof(input);//Convert user input to float
	//NOTE: Arduino does not support strtof, cannot easily validate float input
	return result;
}