#if !defined MOTOR_H
#define MOTOR_H

#include <inttypes.h>
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

  void writePower(int16_t power);
public:
  Motor(){}
  Motor(const Axises& axises, char pin, unsigned int stepMS = 2, unsigned int stepPower = 256)
  {
    pinMode(pin, OUTPUT);
    axises_ = axises;
    pin_ = pin;
    stepMS_ = stepMS;
    stepPower_ = stepPower;
    targetPower_ = 0;
    currentPower_ = 0;
    lastUpdate_ = millis();
  }
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
    currentPower_ += min(targetPower_ - currentPower_, static_cast<int16_t>((millis() - lastUpdate_) / stepMS_ * stepPower_));
    analogWrite(pin_, map(currentPower_, INT16_MIN, INT16_MAX, 0, 255));
    return targetPower_ == currentPower_;
  }

};



#endif