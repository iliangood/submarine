#if !defined (MOTOR_CONTROLLER_H)
#define MOTOR_CONTROLLER_H

#include "HardwareSerial.h"
#include <avr/interrupt.h>

#include <inttypes.h>
#include "motor.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <assert.h>

template<size_t N>
class MotorController
{
  Adafruit_PWMServoDriver pwm;
  Motor<N> motors[N];
  uint16_t freq;

  friend Motor<N>;
public:

  MotorController(uint16_t freq = 50, unsigned int stepMs = 1, unsigned int stepPower = 128) : freq(freq)
  {
    assert(freq <= 500 && "freq out of range");
    assert(N <= 16 && "motor's count too big, should be no more then 16");
    Serial.println("PWM begin");
    pwm.begin();
    pwm.setPWMFreq(freq);
    for(unsigned int i = 0; i < N; ++i)
    {
      motors[i] = Motor<N>(i, this, stepMs, stepPower);
    }
  }

  Motor<N>& operator[](size_t index)
  {
    return motors[index];
  }

  void setAcceleration(const Axises& axises)
  {
    int32_t motorsPower[N];
    int32_t maxValue = 0;
    for(unsigned int i = 0; i < N; ++i)
    {
      maxValue = max(abs(motorsPower[i] = motors[i].getRequiredPower(axises)), maxValue);
    }
    if(maxValue > INT16_MAX)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        motors[i].setPower(((static_cast<int64_t>(motorsPower[i])) * INT16_MAX) / maxValue);
      }
      return;
    }
    for(unsigned int i = 0; i < N; ++i)
    {
      motors[i].setPower(motorsPower[i]);
    }  
  }
  
  void update()
  {
    for(unsigned int  i = 0; i < N; ++i)
      motors[i].update();
  }
};

#endif