#include "HardwareSerial.h"
#if !defined (SENSORS_H)
#define SENSORS_H
#include "MPU6050.h"
#include <MS5837.h>
#include <Wire.h>
#include "axis.h"

class Accelerometer
{
  MPU6050 mpu;
public:
  Accelerometer(TwoWire& wire) : mpu(wire) {}

  void setAxisMapping(MPU6050::Axis x, MPU6050::Axis y, MPU6050::Axis z)
  {
    mpu.setAxisMapping(x, y, z);
  }
  void setAxisInversion(bool invX, bool invY, bool invZ)
  {
    mpu.setAxisInversion(invX, invY, invZ);
  }
  void resetAxisTransform()
  {
    mpu.resetAxisTransform();
  }
  void calcOffsets(bool is_calc_gyro = true, bool is_calc_acc = true) {
    mpu.calcOffsets(is_calc_gyro, is_calc_acc);
  }

  uint8_t init() // Вроде не должно влиять, но что-то не работало и это помогало
  {
    //Serial.println("Accelerometer calibration");
    delay(35);
    volatile uint8_t rc = mpu.begin(0, 0);
    //Serial.println(rc);
    if(rc != 0)
      return rc;
    delay(15);
    //Serial.println('a');
    mpu.calcOffsets();
    return 0;
  }

  Axises getAcceleration()
  {
    return Axises{
      float_range_to_int16(mpu.getAccX(), -360.0, 360.0),
      float_range_to_int16(mpu.getAccY(), -360.0, 360.0),
      float_range_to_int16(mpu.getAccZ(), -360.0, 360.0),
      float_range_to_int16(mpu.getGyroX(), -360.0, 360.0),
      float_range_to_int16(mpu.getGyroY(), -360.0, 360.0),
      float_range_to_int16(mpu.getGyroZ(), -360.0, 360.0)
    };
  }
  Axises getPos()
  {
    return Axises{
      0,
      0,
      0,
      float_range_to_int16(mpu.getAngleX(), -360.0, 360.0),
      float_range_to_int16(mpu.getAngleY(), -360.0, 360.0),
      float_range_to_int16(math_mod(mpu.getAngleZ(), 360.0f) , 0, 360.0)
    };
  }
  void update()
  {
    mpu.update();
  }

};

class DepthGauge
{
  MS5837 sensor;
public:
  bool init(uint8_t mode = 0)
  {
    return sensor.begin(mode);
  }
  void update()
  {
    Serial.println("read");
    sensor.read();
    Serial.println("readed");
  }
  float depth()
  {
    Serial.println("deapth");
    float res = sensor.getDepth();
    Serial.println("deapthed");
    return res;
  }
};

#endif
