#include "PID.h"

int16_t PID::update(int16_t current)
  {
    uint16_t time_elapsed = constrain(millis() - last_update_, 1, 50);

    int32_t diff = static_cast<int32_t>(target_) - static_cast<int32_t>(current);
    
    int16_t p_component = static_cast<int16_t>(constrain(diff * p_, INT16_MIN, INT16_MAX));

    i_component = constrain(i_component + (diff * i_ * time_elapsed), -i_max_, i_max_);
    
    int16_t d_component = constrain(-(static_cast<int32_t>(current) - prev_) * d_ / time_elapsed, INT16_MIN, INT16_MAX);
    
    prev_ = current;
    last_update_ = millis();
    return constrain(static_cast<int32_t>(p_component) + i_component + d_component, INT16_MIN, INT16_MAX);
  }