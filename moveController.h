#if !defined(MOVE_CONTROLLER_H)
#define MOVE_CONTROLLER_H
#include <stdint.h>
#include <math.h>
#include "sensors.h"
#include "motorController.h"
#include "PID.h"

// (-, -, -, крен, тангаж, курс)
// есть стабилизация по глубине
template<size_t N>
class MoveController
{
  Accelerometer& accelerometer_;
  DepthGauge& depthGauge_;
  int16_t target_depth_;
  Axises target_;
  Axises currentPower_;
  int16_t xDepthC_;
  int16_t yDepthC_;
  int16_t zDepthC_;
  PID pids_[4];
public:

  enum PIDs
  {
    depth = 0,
    roll,
    pitch,
    yaw
  };

  MoveController(Accelerometer& acc, DepthGauge& depthGauge) : accelerometer_(acc), depthGauge_(depthGauge), target_depth_(0)
  {
    pids_[PIDs::roll].setDifferenceFunc(PIDs::cycledtDifference);
    pids_[PIDs::pitch].setDifferenceFunc(PIDs::cycledtDifference);
    pids_[PIDs::yaw].setDifferenceFunc(PIDs::cycledtDifference);
  }

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
    int16_t deapth = float_range_to_int16(depthGauge_.depth(), 0, 128);
    Axises res;
    res[Axises::Names::Wx] = pids_[PIDs::roll].update(pos[Axises::Names::Wx]);
    int16_t Wy_speed  = pids_[PIDs::pitch].update(pos[Axises::Names::Wy]);
    int16_t Wz_speed = pids_[PIDs::yaw].update(pos[Axises::Names::Wz]);
    float cosWx = cosf(int16_to_float_range(pos[Axises::Names::Wx], -180, 180));
    float sinWx = sinf(int16_to_float_range(pos[Axises::Names::Wx], -180, 180));
    float cosWy = cosf(int16_to_float_range(pos[Axises::Names::Wy], -90, 90));
    float sinWy = sinf(int16_to_float_range(pos[Axises::Names::Wy], -90, 90));
    res[Axises::Names::Wy] = Wy_speed * cosWx + Wz_speed * sinWx;
    res[Axises::Names::Wz] = Wz_speed * sinWx + Wz_speed * cosWx;
    res[Axises::Names::x] = target_[Axises::Names::x];
    res[Axises::Names::y] = target_[Axises::Names::y];
    res[Axises::Names::z] = target_[Axises::Names::z];
    float xDepthCoof = -sinWy * xDepthC_;
    float yDepthCoof = -(sinWx * cosWy) * yDepthC_;
    float zDepthCoof = -(cosWx * cosWy) * zDepthC_;
    target_depth_ += float_range_to_int16( xDepthCoof * target_[Axises::Names::x] + yDepthCoof * target_[Axises::Names::y] + zDepthCoof * target_[Axises::Names::z], INT16_MIN*3, INT16_MAX*3);
    pids_.setTarget(target_depth_);
    int16_t depth_speed = pids_[PIDs::deapth].update(deapth);
  }

  
};

#endif