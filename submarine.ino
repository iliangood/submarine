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

  //Debug.setDebugLevel(DBG_VERBOSE);
  //Debug.setDebugLevel(DBG_WARNING);
  
  uint64_t last_packet_rx_time_message = 0;
  uint64_t lostPackets = 0;
  uint64_t lastPacketCount = 0;
  uint64_t sendCounter = 0;

  DataTransmitter transmitter(mac, 56728, "submarine");
  message<64> msg;
  if (transmitter.init(ip) != 0)
  {
    //DEBUG_ERROR("transmitter init failed");
    while(1);
  }
  Serial.println("transmitter inited");
  MotorController<6> motors(
    Motor(Axises(100, 101, 102, 103, 104, 105), 1), //TODO: Пока тут временные значения
    Motor(Axises(106, 107, 108, 109, 110, 111), 1),
    Motor(Axises(112, 113, 114, 115, 116, 117), 1),
    Motor(Axises(118, 119, 120, 121, 122, 123), 1),
    Motor(Axises(124, 125, 126, 127, 128, 129), 1),
    Motor(Axises(130, 131, 132, 133, 134, 135), 1));
  while(1)
  {
    Serial.println("Попытка получения пакета");
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
    msg.push(SubmarinePacket{
      static_cast<uint64_t>(millis()),
      static_cast<uint64_t>(last_packet_rx_time_message),
      Axises(),
      Axises()
    });
    msg.push(sendCounter);
  
    transmitter.sendData(msg);
    msg.clear();

    delay(5);
  }
}

void loop() {

}
