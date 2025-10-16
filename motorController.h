#if !defined (MOTOR_CONTROLLER_H)
#define MOTOR_CONTROLLER_H
#include "motor.h"

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