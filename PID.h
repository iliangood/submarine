#if !defined PID_H
#define PID_H
#include <Arduino.h>
#include "fixedPoint.h"

// Всего есть 16 бит, P означа.т сколько бит будет после запятой для итогового числа, T - тип числа
template<typename T, size_t P>
constexpr T toFixedPoint(double num)
{
  return static_cast<T>(round(num * (1LL << P)));
}

// Всего есть 16 бит, PP, IP, DP означа.т сколько бит будет после запятой для каждого параметра
template<size_t PP, size_t IP, size_t DP, size_t IM = INT16_MAX/2>
class PID
{
  int16_t p_;
  int16_t i_;
  int16_t d_;
  int16_t prev_;
  int16_t target_;
  uint32_t last_update_;
  int16_t i_component = 0;
public:
  PID( int16_t p, int16_t i, int16_t d) : p_(p), i_(i), d_(d), prev_(0), target_(0), last_update_(millis()) {}
  void setTarget(int16_t target) 
  {
    target_ = target;
    update();
  }
  int16_t update(int16_t current)
  {
    uint16_t time_elapsed = constrain(millis() - last_update_, 1, 50);
    if(last_update_ > millis())
      time_elapsed = constrain(static_cast<uint64_t>(millis()) + UINT32_MAX - last_update_, 1, 50);

    int32_t diff = static_cast<int32_t>(target_) - static_cast<int32_t>(current);
    
    int16_t p_component = (static_cast<int32_t>(diff) * p_) >> PP;

    i_component = constrain(i_component + (static_cast<int64_t>(diff) * i_ * time_elapsed) >> IP, -IM, IM);
    
    int16_t change_rate = (static_cast<int32_t>(current) - prev_) / time_elapsed;
    int16_t d_component = -((static_cast<int32_t>(change_rate) * d_) >> DP);
    
    prev_ = current;
    return constrain(static_cast<int32_t>(p_component) + i_component + d_component, INT16_MIN, INT16_MAX);
  }
};






#endif