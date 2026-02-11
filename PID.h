#if !defined PID_H
#define PID_H
#include <Arduino.h>

class PID
{
  int16_t p_;
  int16_t i_;
  int16_t d_;
  int16_t prev_;
  int16_t target_;
  uint32_t last_update_;
  int16_t i_component = 0;
  //uint32_t prev_update_;
public:
  PID( int16_t p, int16_t i, int16_t d) : p_(p), i_(i), d_(d), prev_(0), target_(0), /*prev_update_(millis()),*/ last_update_(millis()) {}
  void setTarget(int16_t target);
  void update(int16_t current);
};

#endif