#include "PID.h"

void PID::setTarget(int16_t target) 
{
  target_ = target;
  update();
}

int16_t PID::update(int16_t current)
{
  uint16_t time_elapsed = constrain(millis() - last_update, 1, 32);

  int32_t diff = static_cast<in32_t>(target) - static_cast<in32_t>(current);
  
  int16_t p_component = (static_cast<int32_t>(diff) * p_)/INT16_MAX;

  i_component = constrain(i_component + constrain((static_cast<int64_t>(diff) * i_ * time_elapsed)/INT16_MAX, -1024, 1023), INT16_MIN, INT16_MAX);
  
  int16_t change_rate = (static_cast<int32_t>(current) - prev) * 32 / time_elapsed;
  int16_t d_component = -((static_cast<int32_t>(change_rate) * d_) >> 16);
  
  prev_ = current;
  return constrain(static_cast<int32_t>(p_component) + i_component + d_component, INT16_MIN, INT16_MAX);
}