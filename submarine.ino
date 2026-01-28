//#include <Arduino_DebugUtils.h>

#include <easyEthernetLib.h>
#include <message.h>
#include <utils.h>

#include "axis.h"
#include "motor.h"
#include "motorController.h"
#include "packets.h"
#include "sensors.h"


unsigned char mac[6] = {10, 10, 10, 10, 10, 10};

IPAddress ip(192, 168, 1, 75);

void setup() {
  Serial.begin(115200);
  //Wire.setClock(400000L);
  
  uint64_t last_packet_rx_time_message = 0;
  uint32_t lastSendTime = 0;
  Wire.begin();

  

  Accelerometer acc(Wire);
  if (acc.init()) {
    Serial.println("Failed Accelerometer init");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Calibrated");

  DepthGauge depthGauge;
  if(!depthGauge.init()) 
  {
    Serial.println("Failed depthGauge init");
    while (1) {
      delay(10);
    }
  }

  DataTransmitter transmitter(mac, 56728, "submarine");
  message<64> msg;
  if (transmitter.init(ip) != 0)
  {
    Serial.println("transmitter init: FAIL");
    while(1);
  }

  Serial.println("transmitter init: OK");
  MotorController<6> controller;
  controller[0].axises() = Axises(30000, 30000, 30000, 30000, 30000, 30000);
  controller[1].axises() = Axises(30000, 30000, 30000, 30000, 30000, 30000);
  controller[2].axises() = Axises(30000, 30000, 30000, 30000, 30000, 30000);
  controller[3].axises() = Axises(30000, 30000, 30000, 30000, 30000, 30000);
  controller[4].axises() = Axises(30000, 30000, 30000, 30000, 30000, 30000);
  controller[5].axises() = Axises(30000, 30000, 30000, 30000, 30000, 30000);

  uint32_t lastCycle = millis();
  //uint32_t lastNet = millis();
  while(1)
  {
    depthGauge.update();

    //Serial.println("cyclyng0");
    acc.update();
    receiveInfo rci = transmitter.receiveData(&msg);
    /*if(msg.getSize() > 0)
    {
      Serial.print("get from:");
      Serial.print(rci.remoteIP[0]);
      Serial.print('.');
      Serial.print(rci.remoteIP[1]);
      Serial.print('.');
      Serial.print(rci.remoteIP[2]); 
      Serial.print('.');
      Serial.println(rci.remoteIP[3]);
      Serial.print("size:");
      Serial.println(msg.getSize());
      Serial.print("size should be:");
      Serial.println(ControllerPacket::serializedSize());
    }*/
    
    if(msg.getSize() >= ControllerPacket::serializedSize())
    {
      ControllerPacketBuffer packetBuf = msg.read<ControllerPacketBuffer>();
      ControllerPacket packet = ControllerPacket::deserialize(packetBuf.get());
      controller.setAcceleration(packet.speedTarget);

      /*Serial.print(packet.speedTarget[0]);
      Serial.print(' ');
      Serial.print(packet.speedTarget[1]);
      Serial.print(' ');
      Serial.print(packet.speedTarget[2]);
      Serial.print(' ');
      Serial.print(packet.speedTarget[3]);
      Serial.print(' ');
      Serial.print(packet.speedTarget[4]);
      Serial.print(' ');
      Serial.print(packet.speedTarget[5]);
      Serial.print('\n');*/
      last_packet_rx_time_message = packet.sendTime_ms;
    }
    //Serial.println("cyclyng2");
    msg.clear();
    if(lastSendTime + 10 < millis())
    {
      uint8_t pack[44];
      SubmarinePacket {
        static_cast<uint64_t>(millis()),
        static_cast<uint64_t>(last_packet_rx_time_message),
        acc.getAcceleration(),
        acc.getPos(),
        depthGauge.depth()
        //Axises(1,2,3,4,5,6)
      }.serialize(pack);
      msg.push(pack, SubmarinePacket::serializedSize());
      Serial.println("s1");
      transmitter.sendData(msg);
      //Serial.println("s2");
      msg.clear();

    }
    //Serial.println("cyclyng");
    controller.update();

    delay(max(20 - static_cast<int32_t>(millis() - lastCycle), 0));
    lastCycle = millis();
    //Serial.println("cyclyng4");
  }
}

void loop() {

}
