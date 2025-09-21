#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

const char* magicString = "Submarine";

#define clamp(num, minV, maxV) (max(min((num), (maxV)), (minV)))

enum class AxisesNames
{
    Vx = 0,
    Vy = 1,
    Vz = 2,
    Wx = 3,
    Wy = 4,
    Wz = 5
};

class Axises
{
    int16_t axises[6];
public:
    Axises() : axises{0, 0, 0, 0, 0, 0} {}

    Axises(const int16_t* axises)
    {
        for (char i = 0; i < 6; ++i)
        {
            this->axises[i] = clamp(axises[i], -256, 256);
        }
    }

    int16_t& operator[](int index)
    {
        return axises[index];
    }

    int16_t& operator[](AxisesNames index)
    {
        return axises[static_cast<int>(index)];
    }

    int16_t operator[](AxisesNames index) const
    {
        return axises[static_cast<int>(index)];
    }

    int16_t operator[](int index) const
    {
        return axises[index];
    }

    Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz)
    {
        (*this)[AxisesNames::Vx] = clamp(Vx, -256, 256);
        (*this)[AxisesNames::Vy] = clamp(Vy, -256, 256);
        (*this)[AxisesNames::Vz] = clamp(Vz, -256, 256);
        (*this)[AxisesNames::Wx] = clamp(Wx, -256, 256);
        (*this)[AxisesNames::Wy] = clamp(Wy, -256, 256);
        (*this)[AxisesNames::Wz] = clamp(Wz, -256, 256);
    }

    int getAxis(AxisesNames axis) const
    {
        return axises[static_cast<int>(axis)];
    }

    int getAxis(int axis) const
    {
        return axises[axis];
    }
};

class Motor
{
  char pin;
  int16_t power;
  Axises axises;
public:
  Motor(const Axises& axises, char pin)
  {
    pinMode(pin, OUTPUT);
    this->axises = axises;
    this->pin = pin;
    power = 0;
  }

  int16_t getPower() const
  {
    return power;
  }

  int16_t setPower(int16_t power)
  {
    power = clamp(power, -256, 256);
    analogWrite(pin, map(power, -256, 256, 0, 255));
    return power;
  }

  int16_t getRequiredPower(const Axises& axises) const
  {
    int16_t resPower = 0;
    for(char i = 0; i < 6; ++i)
    {
      resPower += clamp(axises[i] * this->axises[i], -256, 256);
    }
    return resPower;
  }

};

template<size_t N>
class MotorController
{
  Motor motors[N];
public:
  MotorController(const Motor* motors)
  {
    for(unsigned int i = 0; i < N; ++i)
    {
      this->motors[i] = motors[i];
    }
  }

  MotorController(...)
  {
    va_list args;
    va_start(args, N);
    for (size_t i = 0; i < N; ++i) 
    {
      motors[i] = va_arg(args, Motor);
    }
    va_end(args);
  }

  void setAcceleration(const Axises& axises)
  {
    int16_t motorsPower[N];
    uint16_t maxValue = 0;
    for(unsigned int i = 0; i < N; ++i)
    {
      maxValue = max(abs(motorsPower[i] = motors[i].getRequiredPower(axises)), maxValue);
    }
    if(maxValue > 256)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        motors[i].setPower(((static_cast<int32_t>(motorsPower[i])) * 256) / maxValue);
      }
    }
    for(unsigned int i = 0; i < N; ++i)
    {
      motors[i].setPower(motorsPower[i]);
    }  
  }
};

class DataTransmitter {
private:
  byte mac[6];
  IPAddress targetIP;
  EthernetUDP Udp;
  EthernetClient tcpClient;
  EthernetServer tcpServer;
  const char* magicString;
  unsigned int port;
  bool isMaster;

public:
  DataTransmitter(const byte* mac, unsigned int port, const char* magicString, bool master = false) 
    : targetIP(0, 0, 0, 0), tcpServer(port), isMaster(master) 
    {
    this->port = port;
    this->magicString = magicString;
    for (char i = 0; i < 6; ++i) 
      this->mac[i] = mac[i];
  }


  IPAddress sendDiscoveryBroadcast()
  {
    Udp.beginPacket(IPAddress(255, 255, 255, 255), port);
    Udp.write(magicString);
    Udp.write((const uint8_t*)&Ethernet.localIP(), 4);
    Udp.endPacket();
    return Ethernet.localIP();
  }

  IPAddress checkDiscoveryPackets() 
  {
    IPAddress noPartner(0, 0, 0, 0);
    int packetSize = Udp.parsePacket();
    if (packetSize) 
    {
      char buffer[packetSize];
      Udp.read(buffer, packetSize);
      if (strncmp(buffer, magicString, strlen(magicString)) == 0) 
      {
        IPAddress receivedIP;
        memcpy(&receivedIP, buffer + strlen(magicString), 4);
        if (receivedIP != Ethernet.localIP()) 
          return targetIP = receivedIP;
      }
    }
    return noPartner;
  }

  int initSlave()
  {
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    if (Ethernet.begin(mac) == 0)
    {
      Serial.println("Ошибка DHCP");
      return 1;
    }
    Serial.println(Ethernet.localIP());
    if (!Udp.begin(port))
    {
      Serial.println("Ошибка открытия UDP-порта");
      return 1;
    }
    return 0;
  }

  int initMaster() 
  {
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    if (Ethernet.begin(mac) == 0) 
    {
      Serial.println("Ошибка DHCP");
      return 1;
    }
    Serial.println(Ethernet.localIP());
    if (!Udp.begin(port)) 
    {
      Serial.println("Ошибка открытия UDP-порта");
      return 1;
    }
    tcpServer.begin();
    return 0;
  }

  bool confirmConnection() {
    if (targetIP == IPAddress(0, 0, 0, 0)) {
      return false;
    }
    if (isMaster) {
      EthernetClient client = tcpServer.available();
      if (client) {
        client.write("ACK");
        client.stop();
        return true;
      }
      return false;
    } else
    {
      if (tcpClient.connect(targetIP, port))
      {
        char buffer[4];
        int len = tcpClient.read((uint8_t*)buffer, 4);
        if (len > 0 && strncmp(buffer, "ACK", 3) == 0)
        {
          tcpClient.stop();
          return true;
        }
        tcpClient.stop();
      }
      return false;
    }
  }

  IPAddress discoverTarget()
  {
    static unsigned long lastBroadcastTime = 0;
    const unsigned long broadcastInterval = 5000;
    IPAddress noPartner(0, 0, 0, 0);

    if (millis() - lastBroadcastTime > broadcastInterval) 
    {
      sendDiscoveryBroadcast();
      lastBroadcastTime = millis();
    }

    IPAddress foundIP = checkDiscoveryPackets();
    if (confirmConnection() && foundIP != noPartner)
      return foundIP;
    return noPartner;
  }

  void sendData(const byte* data, int dataSize)
  {
    if (targetIP != IPAddress(0, 0, 0, 0))
    {
      Udp.beginPacket(targetIP, port);
      Udp.write(data, dataSize);
      Udp.endPacket();
    }
  }

  int receiveData(byte* buffer, int maxSize)
  {
    int packetSize = Udp.parsePacket();
    if (packetSize && packetSize <= maxSize)
    {
      Udp.read(buffer, packetSize);
      if (strncmp((char*)buffer, magicString, strlen(magicString)) != 0)
        return packetSize;
    }
    return 0;
  }

  IPAddress getTargetIP()
  {
    return targetIP;
  }

  void maintain() //Обновление DHCP
  {
    Ethernet.maintain();
  }
};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
