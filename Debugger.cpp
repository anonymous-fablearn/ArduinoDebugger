#include <ArduinoDebugger.h>
#include <ArduinoDebuggerF.h>
#include <Debugger.h>

static ArduinoDebugger Debugger::initialize(bool usingFloat, bool isAvr, bool usingTerminal)
{
	if(usingFloat)
	{
		ArduinoDebuggerF debug(isAvr, usingTerminal);
		return debug;
	}
	else
	{
		ArduinoDebugger debug(isAvr, usingTerminal);
		return debug;
	}
}