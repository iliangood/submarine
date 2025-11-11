#if !defined MOTOR_H
#define MOTOR_H

#include <inttypes.h>
#include "axis.h"

class Motor
{
  char pin_;
  int16_t targetPower_;
  int16_t currentPower_;
  Axises axises_;
  uint32_t lastUpdate_;
  unsigned int stepMS_;
  unsigned int stepPower_;
public:
  Motor();
  Motor(const Axises& axises, char pin, unsigned int stepMS = 2, unsigned int stepPower = 256);

  int16_t getTargetPower() const;

  int16_t getCurrentPower() const;

  int16_t setPower(int16_t power);

  int32_t getRequiredPower(const Axises& axises) const;

  Axises& axises() { return axises_; }
  const Axises& axises() const { return axises_; }

  bool update();

};



#endif