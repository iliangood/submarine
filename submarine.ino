#include <Arduino_DebugUtils.h>

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
    Motor(Axises(100, 101, 102, 103, 104, 105), 1), //TODO: Пока тут временные значения
    Motor(Axises(106, 107, 108, 109, 110, 111), 1),
    Motor(Axises(112, 113, 114, 115, 116, 117), 1),
    Motor(Axises(118, 119, 120, 121, 122, 123), 1),
    Motor(Axises(124, 125, 126, 127, 128, 129), 1),
    Motor(Axises(130, 131, 132, 133, 134, 135), 1));
  
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
