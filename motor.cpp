#include "motor.h"

Motor::Motor(){}
Motor::Motor(const Axises& axises, char pin, unsigned int stepMS = 2)
{
  pinMode(pin, OUTPUT);
  this->axises = axises;
  this->pin = pin;
  this->stepMS = stepMS;
  targetPower = 0;
  currentPower = 0;
  lastUpdate = millis();
}

int16_t Motor::getTargetPower() const
{
  return currentPower;
}

int16_t Motor::getCurrentPower() const
{
  return targetPower;
}

int16_t Motor::setPower(int16_t power)
{
  targetPower = clamp(power, -256, 256);
  
  return power;
}

int16_t Motor::getRequiredPower(const Axises& axises) const
{
  int16_t resPower = 0;
  for(char i = 0; i < 6; ++i)
  {
    resPower += clamp(axises[i] * this->axises[i], -256, 256);
  }
  return resPower;
}

bool Motor::update()
{
  currentPower += min(targetPower - currentPower, (millis() - lastUpdate) / stepMS);
  analogWrite(pin, map(currentPower, -256, 256, 0, 255));
  return targetPower == currentPower;
}
