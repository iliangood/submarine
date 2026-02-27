#if !defined(MOVE_CONTROLLER_H)
#define MOVE_CONTROLLER_H
#include "sensors.h"
#include "motorController.h"
#include "PID.h"

// (-, -, -, крен, тангаж, курс)
// есть стабилизация по глубине
template<size_t N>
class MoveController
{
  MotorController<N> motorController_;
  Accelerometer& accelerometer_;
  DepthGauge& depthGauge_;
  Axises target_;
  Axises currentPower_;
  int16_t xDepth;
  int16_t yDepth;
  int16_t zDepth;
  PID pids_[4];
public:

  enum PIDs
  {
    depth = 0,
    roll,
    pitch,
    yaw
  };

  MoveController(Accelerometer& acc, DepthGauge& depthGauge) : accelerometer_(acc), depthGauge_(depthGauge) {}

  PID& pid(size_t index)
  {
    return pids_[index];
  }

  const PID& pid(size_t index) const
  {
    return pids_[index];
  }

  void setTarget(const Axises& target)
  {
    target_ = target;
  }

  void update()
  {
    Axises pos = accelerometer_.getPos();
    Axises res;
    res[Axises::Names::Wx] = pids_[PIDs::roll].update(pos[Axises::Names::Wx]);
    res[Axises::Names::Wy] = pids_[PIDs::pitch].update(pos[Axises::Names::Wy]);
    res[Axises::Names::Wz] = pids_[PIDs::yaw].update(pos[Axises::Names::Wz]);
    res[0] = target[0];
    res[1] = target[1];
    res[2] = target[2];
  }

  
};

#endif