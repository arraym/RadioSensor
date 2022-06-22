#include <Arduino.h>
#include "messages.h"
#include "DHT.h"
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>                // Not actually used but needed to compile
#endif

#define MY_ADDRESS  0x01        // This board destination address

#define SENSOR_TIME 3000        // Time to wait for sensor response

// Radio driver defines
#define RX_PIN      11
#define TX_PIN      12
#define PTT_PIN     13          // (unused)

RH_ASK driver(2000, RX_PIN, TX_PIN, PTT_PIN);  
                                // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85), 
DHT dht;
unsigned long startTime;

void setup()
{
#ifdef RH_HAVE_SERIAL
  Serial.begin(115200);	        // Debugging only
#endif
  if (!driver.init())
#ifdef RH_HAVE_SERIAL
      Serial.println("init failed");
#else
	;
#endif
  dht.setup(10);
  delay(dht.getMinimumSamplingPeriod());
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
      {
        startTime = millis();
        float temp = dht.getTemperature();
        while(dht.getStatus())
        {
          if (millis() - startTime > SENSOR_TIME)
          {
            recvCommand.cmd = NO_COMMAND;   // go to default case
            break;
          }
          delay(dht.getMinimumSamplingPeriod());
          temp = dht.getTemperature();
        }
        Serial.print("Temperature: ");
        Serial.print(temp, 2);
        Serial.println("*C");
        r = ACK;
        p = (uint32_t)(temp * 100);
        break;
      }
      case GET_HUMIDITY:
      {
        startTime = millis();
        float hdt = dht.getHumidity();
        while(dht.getStatus())
        {
          if (millis() - startTime > SENSOR_TIME)
          {
            recvCommand.cmd = NO_COMMAND;   // go to default case
            break;
          }
          delay(dht.getMinimumSamplingPeriod());
          hdt = dht.getHumidity();
        }
        Serial.print("Humidity: ");
        Serial.print(hdt, 2);
        Serial.println("%");
        r = ACK;
        p = (uint32_t)(hdt * 100);
        break;
      }
      default:
        Serial.print("Unknown command.");
        r = NACK;
        p = 0;
        break;
      }
      Response myResponse(r, p);
      driver.send(myResponse.responseArray, RESPONSE_SIZE);
    }
  }
}