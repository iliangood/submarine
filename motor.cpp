#include <inttypes.h>
#include "motor.h"

#include <utils.h>

#include <Arduino.h>

Motor::Motor(){}
Motor::Motor(const Axises& axises, char pin, unsigned int stepMS, unsigned int stepPower)
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

int16_t Motor::getTargetPower() const
{
  return targetPower_;
}

int16_t Motor::getCurrentPower() const
{
  return currentPower_;
}

int16_t Motor::setPower(int16_t power)
{
  targetPower_ = power;
  return power;
}

int32_t Motor::getRequiredPower(const Axises& axises) const
{
  int32_t resPower = 0;
  for(char i = 0; i < 6; ++i)
  {
    resPower += axises[i] * axises_[i];
  }
  return resPower;
}

bool Motor::update()
{
  currentPower_ += min(targetPower_ - currentPower_, (millis() - lastUpdate_) / stepMS_ * stepPower_);
  analogWrite(pin_, map(currentPower_, INT16_MIN, INT16_MAX, 0, 255));
  return targetPower_ == currentPower_;
}
