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
  int32_t(*differenceFunc_)(int16_t, int16_t);
public:
  PID( float p, float i, float d, int16_t i_max = 4000) : p_(p), i_(i), d_(d), prev_(0), target_(0), last_update_(millis()), i_max_(i_max), differenceFunc_(defaultDifference) {}
  
  void setTarget(int16_t target) 
  {
    target_ = target;
  }

  void setDifferenceFunc(int32_t(*differenceFunc)(int16_t, int16_t))
  {
    differenceFunc_ = differenceFunc;
  }

  int16_t update(int16_t current);

  void setcoefficients(float p, float i, float d)
  {
    p_ = p;
    i_ = i;
    d_ = d;
  }
  
  static int32_t defaultDifference(int16_t a, int16_t b)
  {
    return static_cast<int32_t>(a)-b;
  }
  
  static int32_t cycledtDifference(int16_t a, int16_t b)
  {
    int32_t diff = static_cast<int32_t>(a) - b;

    if (diff > INT16_MAX)
    {
      diff -= UINT16_MAX+1;
    }
    else if (diff < INT16_MIN)
    {
      diff += UINT16_MAX+1;
    }

    return diff;
  }

};


#endif