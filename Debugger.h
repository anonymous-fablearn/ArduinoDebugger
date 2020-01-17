#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_
#include <Arduino.h>
#include <ArduinoDebugger.h>
#include <ArduinoDebuggerF.h>
class Debugger{
public:

	static ArduinoDebugger initialize(bool usingFloat, bool isAvr, bool usingTerminal);

};
#endif