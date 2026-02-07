#if !defined(MOVE_CONTROLLER_H)
#define MOVE_CONTROLLER_H
#include "sensors.h"
#include "motorController.h"

template<size_t N>
class MoveController
{
  MotorController<N> motorController_;
  const Accelerometer& acc_;
  Axises target_;
  Axises currentPower_;
public:
  MoveController(const Accelerometer& acc);

  void setTarget(const Axises& target);
  void update();
};

#endif