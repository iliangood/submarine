#include <easyEthernetLib.h>
#include <message.h>
#include <utils.h>

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
  int16_t targetPower;
  int16_t currentPower;
  Axises axises;
  uint32_t lastUpdate;
  unsigned int stepMS;
public:
  Motor(){}
  Motor(const Axises& axises, char pin, unsigned int stepMS = 2)
  {
    pinMode(pin, OUTPUT);
    this->axises = axises;
    this->pin = pin;
    this->stepMS = stepMS;
    targetPower = 0;
    currentPower = 0;
    lastUpdate = millis();
  }

  int16_t getTargetPower() const
  {
    return currentPower;
  }

  int16_t getCurrentPower() const
  {
    return targetPower;
  }

  int16_t setPower(int16_t power)
  {
    targetPower = clamp(power, -256, 256);
    
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

  bool update()
  {
    currentPower += min(targetPower - currentPower, (millis() - lastUpdate) / stepMS);
    analogWrite(pin, map(currentPower, -256, 256, 0, 255));
    return targetPower == currentPower;
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

  Motor& operator[](size_t index)
  {
    return motors[index];
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




byte mac[] = {10, 10, 10, 10, 10, 10};

IPAddress ip(192,168,1,2);

void setup() {
  Serial.begin(9600);
  DataTransmitter transmitter(mac, 80, "submarine");
  message<64> receiveBuf;
  message<64> transmittBuf;
  if (transmitter.init(ip) != 0)
  {
    DEBUG_ERROR("Init failed");
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
