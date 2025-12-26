#if !defined MOTOR_H
#define MOTOR_H

#include <inttypes.h>
#include <limits.h>
#include <utils.h>
#include "axis.h"

template<size_t N>
class MotorController;

template<size_t N>
class Motor
{
  char pin_;
  int16_t targetPower_;
  int16_t currentPower_;
  Axises axises_;
  uint32_t lastUpdate_;
  unsigned int stepMS_;
  unsigned int stepPower_;
  MotorController<N>* motorController_;

  void writePower(int16_t power)
  {
    /*Serial.print("write ");
    Serial.print(map(power, INT16_MIN, INT16_MAX, 1000, 2000));
    Serial.print(" to ");
    Serial.println((int)pin_);*/
    motorController_->pwm.writeMicroseconds(pin_, map(power, INT16_MIN, INT16_MAX, 1000, 2000));
  }
public:
  Motor(){}

  Motor(char pin, MotorController<N>* controller, unsigned int stepMs = 1, unsigned int stepPower = 128) : pin_(pin), motorController_(controller), stepMS_(stepMs), stepPower_(stepPower), targetPower_(0), currentPower_(0), lastUpdate_(millis()) {}

  int16_t getTargetPower() const
  {
    return targetPower_;
  }

  int16_t getCurrentPower() const
  {
    return currentPower_;
  }

  int16_t setPower(int16_t power)
  {
    targetPower_ = power;
    return power;
  }

  int32_t getRequiredPower(const Axises& axises) const
  {
    int32_t resPower = 0;
    /*Serial.print("required on ");
    Serial.print((int)pin_);
    Serial.print(":");
    writePower(currentPower_);*/
    for(char i = 0; i < 6; ++i)
    {
      //Serial.print(axises[i]);
      //Serial.print(' ');
      resPower += static_cast<int32_t>(axises[i]) * axises_[i] / INT16_MAX;
    }
    //Serial.print(resPower);
    //Serial.print('\n');
    return resPower;
  }

  Axises& axises() { return axises_; }
  const Axises& axises() const { return axises_; }

  /*bool update()
  {
    int16_t limiter = clamp((millis() - lastUpdate_) / stepMS_ * stepPower_, -INT16_MAX, INT16_MAX);
    currentPower_ += clamp(targetPower_ - currentPower_, -limiter, limiter);
    Serial.print("currentPower_ on ");
    Serial.print((int)pin_);
    Serial.print(":");
    Serial.println(currentPower_);
    writePower(currentPower_);
    lastUpdate_ = millis();
    return targetPower_ == currentPower_;
  }*/

  bool update()
{
    uint32_t now = millis();
    uint32_t deltaTime = now - lastUpdate_;
    lastUpdate_ = now;

    // Вычисляем максимально допустимое изменение мощности
    // Используем 32-битные вычисления, чтобы избежать потери точности
    int32_t maxDelta = (static_cast<int32_t>(deltaTime) * stepPower_) / stepMS_;

    // Ограничиваем по разумным пределам (на всякий случай)
    if (maxDelta > INT16_MAX) maxDelta = INT16_MAX;
    if (maxDelta < -INT16_MAX) maxDelta = -INT16_MAX; // хотя отрицательным не будет

    int16_t desiredChange = targetPower_ - currentPower_;
    int16_t actualChange = clamp(desiredChange, -maxDelta, maxDelta);

    currentPower_ += actualChange;

    // Отладочный вывод
    /*Serial.print("currentPower_ on ");
    Serial.print((int)pin_);
    Serial.print(": ");
    Serial.println(currentPower_);/*/

    writePower(currentPower_);

    return currentPower_ == targetPower_;
}

};



#endif