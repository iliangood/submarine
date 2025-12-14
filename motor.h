#if !defined MOTOR_H
#define MOTOR_H

#include <inttypes.h>
#include <limits.h>
#include "axis.h"

template<size_t N>
class MotorController;

template<size_t N>
class Motor
{
  char pin_;
  int16_t targetPower_;
  int16_t currentPower_;
  Axises axises_;
  uint32_t lastUpdate_;
  unsigned int stepMS_;
  unsigned int stepPower_;
  MotorController<N>* motorController;

  void writePower(int16_t power)
  {
    motorController->pwm.writeMicroseconds(pin_, map(power, INT16_MIN, INT16_MAX, 1000, 2000));
  }
public:
  Motor(){}

  Motor(char pin, unsigned int stepMs = 1, unsigned int stepPower = 128) : pin_(pin), stepMS_(stepMs), stepPower_(stepPower) {}

  int16_t getTargetPower() const
  {
    return targetPower_;
  }

  int16_t getCurrentPower() const
  {
    return currentPower_;
  }

  int16_t setPower(int16_t power)
  {
    targetPower_ = power;
    return power;
  }

  int32_t getRequiredPower(const Axises& axises) const
  {
    int32_t resPower = 0;
    for(char i = 0; i < 6; ++i)
    {
      resPower += axises[i] * axises_[i];
    }
    return resPower;
  }

  Axises& axises() { return axises_; }
  const Axises& axises() const { return axises_; }

  bool update()
  {
    int16_t limiter = clamp((millis() - lastUpdate_) / stepMS_ * stepPower_, -INT16_MAX, INT16_MAX);
    currentPower_ += clamp(targetPower_ - currentPower_, -limiter, limiter);
    writePower(currentPower_);
    return targetPower_ == currentPower_;
  }

};



#endif