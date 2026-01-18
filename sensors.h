#include "HardwareSerial.h"
#if !defined (SENSORS_H)
#define SENSORS_H
#include <MPU6050_light.h>
#include <Wire.h>
#include "axis.h"

class Accelerometer
{
  MPU6050 mpu;
public:
  Accelerometer(TwoWire& wire) : mpu(wire) {}

  uint8_t init() 
  {
    Serial.println("Accelerometer calibration");
    delay(35);
    volatile uint8_t rc = mpu.begin();
    Serial.println(rc);
    if(rc != 0)
      return rc;
    delay(15);
    Serial.println('a');
    mpu.calcOffsets();
    return 0;
  }


  bool& upsideDownMounting() 
  {
    return mpu.upsideDownMounting;
  }

  Axises getAcceleration() 
  {
    return Axises{
      float_range_to_int16(mpu.getAccX(), -180.0, 180.0),
      float_range_to_int16(mpu.getAccY(), -180.0, 180.0),
      float_range_to_int16(mpu.getAccZ(), -180.0, 180.0),
      float_range_to_int16(mpu.getGyroX(), -180.0, 180.0),
      float_range_to_int16(mpu.getGyroY(), -180.0, 180.0),
      float_range_to_int16(mpu.getGyroZ(), -180.0, 180.0)
    };
  }
  Axises getPos() 
  {
    return Axises{
      0,
      0,
      0,
      float_range_to_int16(mpu.getAngleX(), -180.0, 180.0),
      float_range_to_int16(mpu.getAngleY(), -180.0, 180.0),
      float_range_to_int16(mpu.getAngleZ(), -180.0, 180.0)
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
  DepthGauge(TwoWire wire) : sensor(wire) {}
  bool init()
  {
    return sensor.init();
  }
  float depth()
  {
    sensor.read();
    return sensor.depth();
  }
}

#endif
