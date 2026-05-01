#ifndef MPU6050_LIGHT_H
#define MPU6050_LIGHT_H

#include "Arduino.h"
#include "Wire.h"
#include <math.h>

/* Register map:
 * https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
 */

#define MPU6050_ADDR                  0x68

#define MPU6050_SMPLRT_DIV_REGISTER   0x19
#define MPU6050_CONFIG_REGISTER       0x1A
#define MPU6050_GYRO_CONFIG_REGISTER  0x1B
#define MPU6050_ACCEL_CONFIG_REGISTER 0x1C
#define MPU6050_INT_ENABLE_REGISTER   0x38
#define MPU6050_ACCEL_OUT_REGISTER    0x3B
#define MPU6050_TEMP_OUT_REGISTER     0x41
#define MPU6050_GYRO_OUT_REGISTER     0x43
#define MPU6050_PWR_MGMT_1_REGISTER   0x6B
#define MPU6050_WHO_AM_I_REGISTER     0x75

#define RAD_2_DEG             57.29577951308232f
#ifndef DEG_2_RAD
#define DEG_2_RAD             0.01745329251994329576923690768489f
#endif

#define CALIB_OFFSET_NB_MES   500
#define TEMP_LSB_2_DEGREE     340.0f
#define TEMP_LSB_OFFSET       12412.0f

#define DEFAULT_GYRO_COEFF    0.98f

class MPU6050 {
  public:
    enum Axis {
      AXIS_X = 0,
      AXIS_Y = 1,
      AXIS_Z = 2
    };

    // INIT and BASIC FUNCTIONS
    MPU6050(TwoWire &w);
    byte begin(int gyro_config_num = 1, int acc_config_num = 0);

    byte writeData(byte reg, byte data);
    byte readData(byte reg);

    void calcOffsets(bool is_calc_gyro = true, bool is_calc_acc = true);
    void calcGyroOffsets() { calcOffsets(true, false); };
    void calcAccOffsets()  { calcOffsets(false, true); };

    void setAddress(uint8_t addr) { address = addr; };
    uint8_t getAddress() { return address; };

    // MPU CONFIG SETTER
    byte setGyroConfig(int config_num);
    byte setAccConfig(int config_num);

    void setGyroOffsets(float x, float y, float z);
    void setAccOffsets(float x, float y, float z);

    void setFilterGyroCoef(float gyro_coeff);
    void setFilterAccCoef(float acc_coeff) { setFilterGyroCoef(1.0f - acc_coeff); }

    // NEW: AXIS REMAP / INVERSION
    // mapX/mapY/mapZ determine which physical sensor axis becomes logical X/Y/Z.
    // Example: setAxisMapping(AXIS_Y, AXIS_X, AXIS_Z) swaps X and Y.
    void setAxisMapping(Axis x, Axis y, Axis z);
    void setAxisInversion(bool invX, bool invY, bool invZ);
    void resetAxisTransform();

    // MPU CONFIG GETTER
    float getGyroXoffset() { return this->gyroXoffset; };
    float getGyroYoffset() { return this->gyroYoffset; };
    float getGyroZoffset() { return this->gyroZoffset; };

    float getAccXoffset() { return this->accXoffset; };
    float getAccYoffset() { return this->accYoffset; };
    float getAccZoffset() { return this->accZoffset; };

    float getFilterGyroCoef() { return this->filterGyroCoef; };
    float getFilterAccCoef() { return 1.0f - this->filterGyroCoef; };

    // DATA GETTER
    float getTemp() { return this->temp; };

    float getAccX() { return this->accX; };
    float getAccY() { return this->accY; };
    float getAccZ() { return this->accZ; };

    float getGyroX() { return this->gyroX; };
    float getGyroY() { return this->gyroY; };
    float getGyroZ() { return this->gyroZ; };

    float getAccAngleX() { return this->angleAccX; };
    float getAccAngleY() { return this->angleAccY; };

    float getAngleX() { return this->angleX; };
    float getAngleY() { return this->angleY; };
    float getAngleZ() { return this->angleZ; };

    float getPlane();

    // INLOOP UPDATE
    void fetchData(); // user should better call 'update' that includes 'fetchData'
    void update();

    // LEGACY / COMPATIBILITY FLAG
    bool upsideDownMounting = false;

  private:
    TwoWire *wire;
    uint8_t address = MPU6050_ADDR; // 0x68 or 0x69

    float gyro_lsb_to_degsec = 0.0f;
    float acc_lsb_to_g = 0.0f;

    float gyroXoffset = 0.0f, gyroYoffset = 0.0f, gyroZoffset = 0.0f;
    float accXoffset = 0.0f, accYoffset = 0.0f, accZoffset = 0.0f;

    float temp = 0.0f;
    float accX = 0.0f, accY = 0.0f, accZ = 0.0f;
    float gyroX = 0.0f, gyroY = 0.0f, gyroZ = 0.0f;

    float angleAccX = 0.0f, angleAccY = 0.0f;
    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;

    long preInterval = 0;
    float filterGyroCoef = DEFAULT_GYRO_COEFF;

    Axis axisMap[3] = {AXIS_X, AXIS_Y, AXIS_Z};
    bool axisInvert[3] = {false, false, false};

    void readRawData(int16_t &rawAccX, int16_t &rawAccY, int16_t &rawAccZ,
                     int16_t &rawTemp,
                     int16_t &rawGyroX, int16_t &rawGyroY, int16_t &rawGyroZ);

    void applyAxisTransform(float &xAcc, float &yAcc, float &zAcc,
                            float &xGyro, float &yGyro, float &zGyro);
};

#endif