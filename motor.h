#if !defined MOTOR_H
#define MOTOR_H

#include "axis.h"

class Motor
{
  char pin;
  int16_t targetPower;
  int16_t currentPower;
  Axises axises;
  uint32_t lastUpdate;
  unsigned int stepMS;
public:
  Motor();
  Motor(const Axises& axises, char pin, unsigned int stepMS = 2);

  int16_t getTargetPower() const;

  int16_t getCurrentPower() const;

  int16_t setPower(int16_t power);

  int16_t getRequiredPower(const Axises& axises) const;

  bool update();

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

#endif