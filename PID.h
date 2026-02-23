#if !defined PID_H
#define PID_H
#include <Arduino.h>


class PID
{
  float p_;
  float i_;
  float d_;
  int16_t prev_;
  int16_t target_;
  uint32_t last_update_;
  int16_t i_component = 0;
  int16_t i_max_;
public:
  PID( float p, float i, float d, int16_t i_max = 4000) : p_(p), i_(i), d_(d), prev_(0), target_(0), last_update_(millis()), i_max_(i_max) {}
  
  void setTarget(int16_t target) 
  {
    target_ = target;
  }

  int16_t update(int16_t current);

  void setcoefficients(float p, float i, float d)
  {
    p_ = p;
    i_ = i;
    d_ = d;
  }

};


#endif