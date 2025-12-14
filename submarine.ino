//#include <Arduino_DebugUtils.h>

#include <easyEthernetLib.h>
#include <message.h>
#include <utils.h>

#include "axis.h"
#include "motor.h"
#include "motorController.h"
#include "packets.h"


byte mac[] = {10, 10, 10, 10, 10, 10};

IPAddress ip(192,168,1,75);

void setup() {
  Serial.begin(115200);
  
  uint64_t last_packet_rx_time_message = 0;
  uint64_t lostPackets = 0;
  uint64_t lastPacketCount = 0;
  uint64_t sendCounter = 0;
  uint32_t lastSendTime = 0;

  DataTransmitter transmitter(mac, 56728, "submarine");
  message<64> msg;
  if (transmitter.init(ip) != 0)
  {
    Serial.println("transmitter init: FAIL");
    while(1);
  }
  //Serial.println("transmitter init: OK");
  MotorController<6> controller;
  controller[0].axises() = Axises(1, 2, 3, 4, 5, 6);
  controller[1].axises() = Axises(7, 8, 9, 10, 11, 12);
  controller[2].axises() = Axises(13, 14, 15, 16, 17, 18);
  controller[3].axises() = Axises(19, 20, 21, 22, 23, 24);
  controller[4].axises() = Axises(25, 26, 27, 28, 29, 30);
  controller[5].axises() = Axises(31, 32, 33, 34, 35, 36);
  while(1)
  {

    receiveInfo rci = transmitter.receiveData(&msg);
    if(msg.getSize() > 0)
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
    }
    if(msg.getSize() > ControllerPacket::serializedSize())
    {
      ControllerPacketBuffer packetBuf = msg.read<ControllerPacketBuffer>();
      ControllerPacket packet = ControllerPacket::deserialize(packetBuf.get());
      controller.setAcceleration(packet.speedTarget);

      Serial.print(packet.speedTarget[0]);
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
      Serial.print('\n');
      last_packet_rx_time_message = packet.sendTime_ms;
      Serial.print("packet.sendTime_ms:");
      Serial.println((uint32_t)packet.sendTime_ms);
      Serial.print("lost packets:");
      uint64_t packetCount = msg.read<uint64_t>();
      Serial.println((uint32_t)(lostPackets += packetCount - lastPacketCount - 1));
      lastPacketCount = packetCount;
    }
    msg.clear();
    if(lastSendTime + 10 < millis())
    {
    msg.push(SubmarinePacket{
      static_cast<uint64_t>(millis()),
      static_cast<uint64_t>(last_packet_rx_time_message),
      Axises(),
      Axises()
    });
    msg.push(sendCounter++);
  
    transmitter.sendData(msg);
    msg.clear();

    }

    delay(15);
  }
}

void loop() {

}
