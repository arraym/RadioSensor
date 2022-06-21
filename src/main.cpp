#include <Arduino.h>
#include "messages.h"
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>                  // Not actually used but needed to compile
#endif

#define MY_ADDRESS    0x01        // This board destination address

RH_ASK driver(2000, 11, 12, 13);  // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85), 

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
    Command recvCommand(buf);
    if(recvCommand.destAddress == MY_ADDRESS)
    {
      Response_TypeDef r = ACK;
      uint32_t p = 0;
      switch (recvCommand.cmd)
      {
      case GET_TEMPERATURE:
        Serial.println("Temperature command received.");
        r = ACK;
        p = 27;
        break;
      case GET_HUMIDITY:
        Serial.println("Humidity command received.");
        r = ACK;
        p = 80;
        break;
      default:
        Serial.print("Unknown command.");
        r = NACK;
        p = 0;
        break;
      }
      Response myResponse(r, p);
      driver.send(myResponse.responseArray, RESPONSE_SIZE);
      driver.printBuffer("Sent: ", myResponse.responseArray, RESPONSE_SIZE);
    }
  }
}