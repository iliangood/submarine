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
    int16_t Wy_speed  = pids_[PIDs::pitch].update(pos[Axises::Names::Wy]);
    int16_t Wz_speed = pids_[PIDs::yaw].update(pos[Axises::Names::Wz]);

    float fWx = int16_to_float_range(pos[Axises::Names::Wx], -180, 180);
    float fWy = int16_to_float_range(pos[Axises::Names::Wy], -90, 90);
    
    float cosWx = cosf(fWx);
    float sinWx = sinf(fWx);
    float cosWy = cosf(fWy);
    float sinWy = sinf(fWy);

    float xDepthCoof = -sinWy * xDepthC_;
    float yDepthCoof = -(sinWx * cosWy) * yDepthC_;
    float zDepthCoof = -(cosWx * cosWy) * zDepthC_;

    target_depth_ += float_range_to_int16( xDepthCoof * target_[Axises::Names::x] + yDepthCoof * target_[Axises::Names::y] + zDepthCoof * target_[Axises::Names::z], INT16_MIN*3, INT16_MAX*3);
    pids_[PIDs::depth].setTarget(target_depth_);
    int16_t depth_speed = pids_[PIDs::deapth].update(deapth);

    res[Axises::Names::x] = constrain(static_cast<int32_t>(target_[Axises::Names::x]) + xDepthCoof * depth_speed, INT16_MIN, INT16_MAX);
    res[Axises::Names::y] = constrain(static_cast<int32_t>(target_[Axises::Names::y]) + yDepthCoof * depth_speed, INT16_MIN, INT16_MAX);
    res[Axises::Names::z] = constrain(static_cast<int32_t>(target_[Axises::Names::z]) + zDepthCoof * depth_speed, INT16_MIN, INT16_MAX);

    res[Axises::Names::Wx] = pids_[PIDs::roll].update(pos[Axises::Names::Wx]);
    res[Axises::Names::Wy] = Wy_speed * cosWx + Wz_speed * sinWx;
    res[Axises::Names::Wz] = Wz_speed * sinWx + Wz_speed * cosWx;
  }

  
};

#endif