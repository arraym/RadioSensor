#include <Arduino.h>
#include "messages.h"
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

RH_ASK driver(2000, 11, 12, 13); // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85), 

void setup()
{
#ifdef RH_HAVE_SERIAL
  Serial.begin(115200);	  // Debugging only
#endif
  if (!driver.init())
#ifdef RH_HAVE_SERIAL
      Serial.println("init failed");
#else
	;
#endif
}

void loop()
{
  uint8_t buf[COMMAND_SIZE];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) // Non-blocking
  {
    // Message with a good checksum received, dump it.
    driver.printBuffer("Got:", buf, buflen);
  }
}