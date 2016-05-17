// Do not remove the include below

#include <Arduino.h>
#include <SerialCommand.h>
#include <Timer.h>
#include "BlinkTimerControl.h"

//-----------------------------------------------------------------------------

void toggleLed(int ledPin)
{
  bool isLedOn = digitalRead(ledPin);
  digitalWrite(ledPin, !isLedOn);
}

// global variable definition
BlinkTimerControl* blinkTimerControl = 0;

// global constant definition
const unsigned int BLINK_TIME_MILLIS = 200;

class BlinkTimerAdapter : public TimerAdapter
{
public:
  void timeExpired()
  {
    toggleLed(LED_BUILTIN);

    if (0 != blinkTimerControl)
    {
      blinkTimerControl->timeExpired();
    }
  }
};

//-----------------------------------------------------------------------------
// Arduino Cmd I/F
//-----------------------------------------------------------------------------
SerialCommand* sCmd = 0;

void decr()
{
  char *arg;
  if (0 != sCmd)
  {
    arg = sCmd->next();    // Get the next argument from the SerialCommand object buffer
  }
  else
  {
    arg = "";
  }
  if (0 != blinkTimerControl)
  {
    if (arg != NULL)         // As long as it exists, take it
    {
      int n = atoi(arg);
      while (n > 0)
      {
        n--;
        blinkTimerControl->decrementBlinkTime();
      }
    }
    else
    {
      blinkTimerControl->decrementBlinkTime();
    }
  }
}

void incr()
{
  char *arg;
  if (0 != sCmd)
  {
    arg = sCmd->next();    // Get the next argument from the SerialCommand object buffer
  }
  else
  {
    arg = "";
  }
  if (0 != blinkTimerControl)
  {
    if (arg != NULL)         // As long as it exists, take it
    {
      int n = atoi(arg);
      while (n > 0)
      {
        n--;
        blinkTimerControl->incrementBlinkTime();
      }
    }
    else
    {
      blinkTimerControl->incrementBlinkTime();
    }
  }
}

// This is the default handler, and gets called when no other command matches.
void unrecognized(const char *command)
{
  Serial.println("What?");
}

//-----------------------------------------------------------------------------

//The setup function is called once at startup of the sketch
void setup()
{
  //-----------------------------------------------------------------------------
  // Serial Command Object for Debug CLI
  //-----------------------------------------------------------------------------
  Serial.begin(115200);
  sCmd = new SerialCommand();

  // Setup callbacks for SerialCommand commands
  if (0 != sCmd)
  {
    sCmd->addCommand("i", incr);
    sCmd->addCommand("d", decr);
    sCmd->setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  }

  Serial.println(F("Hello from Arduino Blink!\n"));

  //---------------------------------------------------------------------------
  // configure LED pin
  //---------------------------------------------------------------------------
  pinMode(LED_BUILTIN, OUTPUT);

  //---------------------------------------------------------------------------
  // setup blink timer control
  //---------------------------------------------------------------------------
  BlinkTimerAdapter* blinkTimerAdapter = new BlinkTimerAdapter();
  blinkTimerControl = new BlinkTimerControl(blinkTimerAdapter, BLINK_TIME_MILLIS);
}

// The loop function is called in an endless loop
void loop()
{
  if (0 != sCmd)
  {
    sCmd->readSerial();                       // process serial commands
  }
  yield();
}
