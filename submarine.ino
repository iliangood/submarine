#include <easyEthernetLib.h>
#include <message.h>
#include <utils.h>

#include "axis.h"
#include "motor.h"
#include "motorController.h"



byte mac[] = {10, 10, 10, 10, 10, 10};

IPAddress ip(192,168,1,2);

void setup() {
  Serial.begin(9600);
  DataTransmitter transmitter(mac, 80, "submarine");
  message<64> receiveBuf;
  message<64> transmittBuf;
  if (transmitter.init(ip) != 0)
  {
    DEBUG_ERROR("transmitter init failed");
    while(1);
  }
  MotorController<6> motors(
    Motor(Axises(100, 100, 100, 100, 100, 100), 1), //TODO: Пока тут временные значения
    Motor(Axises(100, 100, 100, 100, 100, 100), 1),
    Motor(Axises(100, 100, 100, 100, 100, 100), 1),
    Motor(Axises(100, 100, 100, 100, 100, 100), 1),
    Motor(Axises(100, 100, 100, 100, 100, 100), 1),
    Motor(Axises(100, 100, 100, 100, 100, 100), 1));
  while(1)
  {
    transmitter.receiveData(&receiveBuf);
    if(receiveBuf.getSize() == sizeof(Axises))
    {
      motors.setAcceleration(*(Axises*)receiveBuf.getData());
    }
    receiveBuf.clear();
    transmittBuf.push("temp");
    transmitter.sendData(transmittBuf);
  }
}

void loop() {

}
