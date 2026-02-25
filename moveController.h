#if !defined(MOVE_CONTROLLER_H)
#define MOVE_CONTROLLER_H
#include "sensors.h"
#include "motorController.h"
#include "PID.h"

// (-, -, глубина, крен, тангаж, курс)

template<size_t N>
class MoveController
{
  MotorController<N> motorController_;
  const Accelerometer& acc_;
  Axises target_;
  Axises currentPower_;
  PID pids_[6];
public:
  MoveController(const Accelerometer& acc);

  void setTarget(const Axises& target)
  {
    target_ = target;
    
  }
  void update();
};

#endif