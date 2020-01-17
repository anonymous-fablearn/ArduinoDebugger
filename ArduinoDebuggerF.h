#ifndef _ARDUINO_DEBUGGER_F_H_
#define _ARDUINO_DEBUGGER_F_H_
#include <Arduino.h>
#include <ArduinoDebugger.h>
class ArduinoDebuggerF : public ArduinoDebugger{
public:
  void displayVariables();
	void updateVariables();
	ArduinoDebuggerF(bool isAvr, bool usingTerminal) : ArduinoDebugger(isAvr, usingTerminal){};

private:
  float getFloat();	
	void displayArray(Variable var);
	void updateArray(Variable var);
};
#endif