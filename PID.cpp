#include "PID.h"

void PID::setTarget(int16_t target) 
{
  target_ = target;
  update();
}

int16_t PID::update(int16_t current)
{
  uint16_t time_collapsed = constrain(millis() - last_update, 0, UINT16_MAX);
  int32_t diff = static_cast<in32_t>(target) - static_cast<in32_t>(current);
  int32_t prev_diff = static_cast<in32_t>(target) - static_cast<in32_t>(prev_);
  int16_t p_component = (diff * p_)/INT16_MAX;
  i_component = constrain(i_component + (diff * i_)/INT16_MAX, INT16_MIN, INT16_MAX);
  int16_t d_component = (prev_diff - diff)/diff*d_ 
  prev_ = current_;
  return p_component + i_component + d_component;
}