#if defined !(SENSORS_H)
#define SENSORS_H
#include <MPU6050_light.h>
#include <Wire.h>
class accelerometer
{
  MPU6050 mpu;
public:
  accelerometer(TwoWire wire) : mpu(wire) {}
  uint8_t init() {
    Serial.println("Калибровка акселерометра держите датчик неподвижно горизонтально!");
    uint8_t rc = mpu.begin();
    mpu.calcOffsets();
  }
  Axises getMove() {
    mpu.
  }
};
#endif
