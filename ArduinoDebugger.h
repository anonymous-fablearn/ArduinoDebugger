#ifndef _ARDUINO_DEBUGGER_H_
#define _ARDUINO_DEBUGGER_H_
#include <Arduino.h>

enum Type
{
		BYTE,
		BYTE_ARRAY,
		INT,
		INT_ARRAY,
		LONG,
		LONG_ARRAY,
		FLOAT,
		FLOAT_ARRAY,
		CHAR,
		CHAR_ARRAY,
		BOOL,
		BOOL_ARRAY
};
struct Variable
{
	void* ptr;
	Type type;
	char name[16];
};

class ArduinoDebugger{
public:
	
	ArduinoDebugger(bool isAvr, bool usingTerminal);
	
	void breakpoint();
	
	void breakpoint(char name[]);
	
	//Idea allow student to choose subset of digital & analog pins
	//template <size_t n>
	//void breakpoint(char name[], byte (&digital)[n], byte (&analog)[n]);
	
	void add(void* var_ptr, Type type, char var_name[]);
	
	//necessary when dealing with variables that go out of scope!
	void remove(char var_name[]);

	void displayPins();
	void displayPins(byte digital_pins[], byte analog_pins[]);
	void updatePins();
	
	virtual void displayVariables();
	virtual void updateVariables();
	
	void updateTimer();
	
	void clearBuffer();
	
	//Get data type values from Serial Input
	long getNumber(bool &valid, Type type);
	char getChar();
	bool getBool(bool &valid);
	void pause();
	
	void clearScreen();
	
	void drawStars();
	
protected:
	//variable watch
	Variable var_watch[10];
	byte top_var_watch = 0;
	
	bool _isAvr;
	bool _usingTerminal;
	
	//Array helper methods
	void printName(char name[]);
	byte getSize(char name[]);
	byte getNumBytes(Type type);
	virtual void displayArray(Variable var);
	virtual void updateArray(Variable var);
	
	byte getSelection();
	int timedPeek();
	
};

#endif