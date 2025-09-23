#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define DEBUG_LEVEL_NOTHING 0
#define DEBUG_LEVEL_ERRORS 1
#define DEBUG_LEVEL_WARNINGS 2
#define DEBUG_LEVEL_INFO 3

#define DEBUG_LEVEL DEBUG_LEVEL_INFO

#define LOG_ERROR(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_ERRORS) { \
        Serial.print("[ERROR] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

#define LOG_WARNING(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_WARNINGS) { \
        Serial.print("[WARNING] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

#define LOG_INFO(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_INFO) { \
        Serial.print("[INFO] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

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

#define USE_DHCP false

class DataTransmitter {
private:
  uint8_t mac[6];
  IPAddress targetIP;
  EthernetUDP Udp;
  const char* magicString;
  size_t magicStringLength;
  unsigned int port;
#if not USE_DHCP
  IPAddress ip;
#endif
public:
#if USE_DHCP
  DataTransmitter(const byte* mac, unsigned int port, const char* magicString) 
    : targetIP(255, 255, 255, 255) 
    {
    this->port = port;
    this->magicString = magicString;
    if(magicString != nullptr)
      magicStringLength = strlen(magicString);
    if(mac != nullptr)
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = mac[i];
    }
    else
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = 0;
    }
    LOG_INFO("Создан класс DataTransmitter");
  }
#else
  DataTransmitter(const byte* mac, unsigned int port, IPAddress ip, const char* magicString) 
    : targetIP(255, 255, 255, 255), ip(ip)
    {
    this->port = port;
    this->magicString = magicString;
    if(magicString != nullptr)
      magicStringLength = strlen(magicString);
    if(mac != nullptr)
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = mac[i];
    }
    else
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = 0;
    }
    LOG_INFO("Создан класс DataTransmitter");
  }
#endif
  bool isValid()
  {
    if(magicString == nullptr)
      return false;
    bool res = false;
    for(char i = 0; i < 6; ++i)
      res = res || mac[i];
    if(!res)
      return false;
    return true;
  }

  int init()
  {
    return !isValid();
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
#if USE_DHCP
    if (Ethernet.begin(mac) == 0)
    {
      LOG_ERROR("Ошибка DHCP");
      return 1;
    }
#else
    if (Ethernet.begin(mac, ip) == 0)
    {
      LOG_ERROR("Ошибка DHCP");
      return 1;
    }
#endif
    if (!Udp.begin(port))
    {
      LOG_ERROR("Ошибка открытия UDP-порта");
      return 1;
    }
    return 0;
  }

  int sendData(const byte* data, int dataSize)
  {
    if(data == nullptr)
      return 1;
    Udp.beginPacket(targetIP, port);
    Udp.write(magicString);
    Udp.write(data, dataSize);
    Udp.endPacket();
    LOG_INFO("Пакет отправлен");
    return 0;
  }
  int sendData(const char* data)
  {
    sendData((const byte*)data, strlen(data));
  }

  size_t receiveData(byte* buffer, int maxSize)
  {
    if(buffer == nullptr)
    return 0;
    int packetSize = Udp.parsePacket();
    if ((packetSize < 1) || packetSize > maxSize || packetSize < magicStringLength + 1)
      return 0;
    Udp.read(buffer, packetSize);
    if (strncmp((char*)buffer, magicString, magicStringLength) != 0)
      return 0;
#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
    if(targetIP != Udp.remoteIP())
      LOG_INFO("IP аддресс удаленного устройства обновлен");
#endif
    targetIP = Udp.remoteIP();
    memmove(buffer, &buffer[magicStringLength+1], packetSize - magicStringLength - 1);
    LOG_INFO("Пакет получен");
    return packetSize - magicStringLength - 1;
  }

  IPAddress getTargetIP()
  {
    return targetIP;
  }
#if not USE_DHCP
  void maintain() //Обновление DHCP
  {
    LOG_INFO("Обновление DHCP");
    Ethernet.maintain();
  }
#endif
};

byte mac1[] = {10, 10, 10, 10, 10, 10};
byte mac2[] = {10, 10, 10, 10, 10, 11};

IPAddress ip1(192,168,1,2);
IPAddress ip2(192,168,1,3);

void setup() {
#if DEBUG_LEVEL > 0
  Serial.begin(9600);
#endif
  DataTransmitter transmitter(mac1, 80, ip1, "tester");
  byte buf[64];
  while(1)
  {
    delay(100);
    transmitter.sendData("hello world!");
    delay(100);
    size_t size = transmitter.receiveData(buf, 64);
    if(size > 0)
    {
      LOG_INFO((const char*)buf);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
