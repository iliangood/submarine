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



#endif