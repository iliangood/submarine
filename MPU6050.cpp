#include "MPU6050.h"

MPU6050::MPU6050(TwoWire &w) {
  wire = &w;
}

byte MPU6050::begin(int gyro_config_num, int acc_config_num) {
  wire->begin();
  delay(100);

  wire->beginTransmission(address);
  byte status = wire->endTransmission();
  if (status != 0) {
    return 1;
  }

  // Wake up device
  writeData(MPU6050_PWR_MGMT_1_REGISTER, 0x00);
  delay(200);


  // Basic configuration
  writeData(MPU6050_SMPLRT_DIV_REGISTER, 0x07);
  writeData(MPU6050_CONFIG_REGISTER, 0x00);

  if (setGyroConfig(gyro_config_num) != 0) return 2;
  if (setAccConfig(acc_config_num) != 0) return 3;

  // Reset runtime state
  angleX = angleY = angleZ = 0.0f;
  angleAccX = angleAccY = 0.0f;
  temp = 0.0f;
  accX = accY = accZ = 0.0f;
  gyroX = gyroY = gyroZ = 0.0f;

  preInterval = millis();
  return 0;
}

byte MPU6050::writeData(byte reg, byte data) {
  wire->beginTransmission(address);
  wire->write(reg);
  wire->write(data);
  return wire->endTransmission();
}

byte MPU6050::readData(byte reg) {
  wire->beginTransmission(address);
  wire->write(reg);
  if (wire->endTransmission(false) != 0) {
    return 0;
  }

  if (wire->requestFrom(address, (uint8_t)1) != 1) {
    return 0;
  }

  return wire->read();
}

byte MPU6050::setGyroConfig(int config_num) {
  if (config_num < 0) config_num = 0;
  if (config_num > 3) config_num = 3;

  byte reg_value = (byte)(config_num << 3);
  if (writeData(MPU6050_GYRO_CONFIG_REGISTER, reg_value) != 0) {
    return 1;
  }

  switch (config_num) {
    case 0: gyro_lsb_to_degsec = 131.0f; break;
    case 1: gyro_lsb_to_degsec = 65.5f;  break;
    case 2: gyro_lsb_to_degsec = 32.8f;  break;
    case 3: gyro_lsb_to_degsec = 16.4f;  break;
  }

  return 0;
}

byte MPU6050::setAccConfig(int config_num) {
  if (config_num < 0) config_num = 0;
  if (config_num > 3) config_num = 3;

  byte reg_value = (byte)(config_num << 3);
  if (writeData(MPU6050_ACCEL_CONFIG_REGISTER, reg_value) != 0) {
    return 1;
  }

  switch (config_num) {
    case 0: acc_lsb_to_g = 16384.0f; break; // +-2g
    case 1: acc_lsb_to_g = 8192.0f;  break; // +-4g
    case 2: acc_lsb_to_g = 4096.0f;  break; // +-8g
    case 3: acc_lsb_to_g = 2048.0f;  break; // +-16g
  }

  return 0;
}

void MPU6050::setGyroOffsets(float x, float y, float z) {
  gyroXoffset = x;
  gyroYoffset = y;
  gyroZoffset = z;
}

void MPU6050::setAccOffsets(float x, float y, float z) {
  accXoffset = x;
  accYoffset = y;
  accZoffset = z;
}

void MPU6050::setFilterGyroCoef(float gyro_coeff) {
  if (gyro_coeff < 0.0f) gyro_coeff = 0.0f;
  if (gyro_coeff > 1.0f) gyro_coeff = 1.0f;
  filterGyroCoef = gyro_coeff;
}

void MPU6050::setAxisMapping(Axis x, Axis y, Axis z) {
  axisMap[0] = x;
  axisMap[1] = y;
  axisMap[2] = z;
}

void MPU6050::setAxisInversion(bool invX, bool invY, bool invZ) {
  axisInvert[0] = invX;
  axisInvert[1] = invY;
  axisInvert[2] = invZ;
}

void MPU6050::resetAxisTransform() {
  axisMap[0] = AXIS_X;
  axisMap[1] = AXIS_Y;
  axisMap[2] = AXIS_Z;

  axisInvert[0] = false;
  axisInvert[1] = false;
  axisInvert[2] = false;
}

void MPU6050::readRawData(int16_t &rawAccX, int16_t &rawAccY, int16_t &rawAccZ,
                          int16_t &rawTemp,
                          int16_t &rawGyroX, int16_t &rawGyroY, int16_t &rawGyroZ) {
  wire->beginTransmission(address);
  wire->write(MPU6050_ACCEL_OUT_REGISTER);
  wire->endTransmission(false);

  wire->requestFrom(address, (uint8_t)14);

  if (wire->available() >= 14) {
    rawAccX = (int16_t)(wire->read() << 8 | wire->read());
    rawAccY = (int16_t)(wire->read() << 8 | wire->read());
    rawAccZ = (int16_t)(wire->read() << 8 | wire->read());
    rawTemp = (int16_t)(wire->read() << 8 | wire->read());
    rawGyroX = (int16_t)(wire->read() << 8 | wire->read());
    rawGyroY = (int16_t)(wire->read() << 8 | wire->read());
    rawGyroZ = (int16_t)(wire->read() << 8 | wire->read());
  }
}

void MPU6050::applyAxisTransform(float &xAcc, float &yAcc, float &zAcc,
                                 float &xGyro, float &yGyro, float &zGyro) {
  float inAcc[3]  = {xAcc, yAcc, zAcc};
  float inGyro[3] = {xGyro, yGyro, zGyro};

  float outAcc[3];
  float outGyro[3];

  for (int i = 0; i < 3; i++) {
    outAcc[i]  = inAcc[axisMap[i]];
    outGyro[i] = inGyro[axisMap[i]];

    if (axisInvert[i]) {
      outAcc[i]  = -outAcc[i];
      outGyro[i] = -outGyro[i];
    }
  }

  xAcc = outAcc[0];
  yAcc = outAcc[1];
  zAcc = outAcc[2];

  xGyro = outGyro[0];
  yGyro = outGyro[1];
  zGyro = outGyro[2];
}

void MPU6050::calcOffsets(bool is_calc_gyro, bool is_calc_acc) {
  float sumAccX = 0.0f, sumAccY = 0.0f, sumAccZ = 0.0f;
  float sumGyroX = 0.0f, sumGyroY = 0.0f, sumGyroZ = 0.0f;

  float oldAccXoffset = accXoffset;
  float oldAccYoffset = accYoffset;
  float oldAccZoffset = accZoffset;
  float oldGyroXoffset = gyroXoffset;
  float oldGyroYoffset = gyroYoffset;
  float oldGyroZoffset = gyroZoffset;

  // temporarily clear offsets so calibration is based on raw sensor values
  accXoffset = accYoffset = accZoffset = 0.0f;
  gyroXoffset = gyroYoffset = gyroZoffset = 0.0f;

  for (int i = 0; i < CALIB_OFFSET_NB_MES; i++) {
    int16_t rawAccX = 0, rawAccY = 0, rawAccZ = 0;
    int16_t rawTemp = 0;
    int16_t rawGyroX = 0, rawGyroY = 0, rawGyroZ = 0;

    readRawData(rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ);

    float aX = (float)rawAccX / acc_lsb_to_g;
    float aY = (float)rawAccY / acc_lsb_to_g;
    float aZ = (float)rawAccZ / acc_lsb_to_g;

    float gX = (float)rawGyroX / gyro_lsb_to_degsec;
    float gY = (float)rawGyroY / gyro_lsb_to_degsec;
    float gZ = (float)rawGyroZ / gyro_lsb_to_degsec;

    applyAxisTransform(aX, aY, aZ, gX, gY, gZ);

    if (is_calc_acc) {
      sumAccX += aX;
      sumAccY += aY;
      sumAccZ += aZ;
    }

    if (is_calc_gyro) {
      sumGyroX += gX;
      sumGyroY += gY;
      sumGyroZ += gZ;
    }

    delay(2);
  }

  if (is_calc_acc) {
    float meanAccX = sumAccX / CALIB_OFFSET_NB_MES;
    float meanAccY = sumAccY / CALIB_OFFSET_NB_MES;
    float meanAccZ = sumAccZ / CALIB_OFFSET_NB_MES;

    // Assume sensor is stationary during calibration.
    // In normal orientation expected gravity on Z is +1g.
    // In upsideDownMounting mode expected Z is -1g.
    float expectedZ = upsideDownMounting ? -1.0f : 1.0f;

    accXoffset = meanAccX;
    accYoffset = meanAccY;
    accZoffset = meanAccZ - expectedZ;
  }

  if (is_calc_gyro) {
    gyroXoffset = sumGyroX / CALIB_OFFSET_NB_MES;
    gyroYoffset = sumGyroY / CALIB_OFFSET_NB_MES;
    gyroZoffset = sumGyroZ / CALIB_OFFSET_NB_MES;
  }

  // Restore offsets? No: keep newly calculated ones.
  // If only one type was calibrated, restore the other type.
  if (!is_calc_acc) {
    accXoffset = oldAccXoffset;
    accYoffset = oldAccYoffset;
    accZoffset = oldAccZoffset;
  }
  if (!is_calc_gyro) {
    gyroXoffset = oldGyroXoffset;
    gyroYoffset = oldGyroYoffset;
    gyroZoffset = oldGyroZoffset;
  }

  // Reset timing to avoid a large first dt after calibration
  preInterval = millis();
}

void MPU6050::fetchData() {
  int16_t rawAccX = 0, rawAccY = 0, rawAccZ = 0;
  int16_t rawTemp = 0;
  int16_t rawGyroX = 0, rawGyroY = 0, rawGyroZ = 0;

  readRawData(rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ);

  float aX = (float)rawAccX / acc_lsb_to_g;
  float aY = (float)rawAccY / acc_lsb_to_g;
  float aZ = (float)rawAccZ / acc_lsb_to_g;

  float gX = (float)rawGyroX / gyro_lsb_to_degsec;
  float gY = (float)rawGyroY / gyro_lsb_to_degsec;
  float gZ = (float)rawGyroZ / gyro_lsb_to_degsec;

  // Apply user axis transform BEFORE offsets and angle calculation
  applyAxisTransform(aX, aY, aZ, gX, gY, gZ);

  // Apply offsets
  accX = aX - accXoffset;
  accY = aY - accYoffset;
  accZ = aZ - accZoffset;

  gyroX = gX - gyroXoffset;
  gyroY = gY - gyroYoffset;
  gyroZ = gZ - gyroZoffset;

  // Temperature
  temp = (float)rawTemp / TEMP_LSB_2_DEGREE + 36.53f;

  // Acceleration-based angles
  angleAccX = atan2f(accY, accZ) * RAD_2_DEG;
  angleAccY = atan2f(-accX, sqrtf(accY * accY + accZ * accZ)) * RAD_2_DEG;

  // Legacy compatibility flag
  if (upsideDownMounting) {
    angleAccX = -angleAccX;
    angleAccY = -angleAccY;
  }

  unsigned long now = millis();
  if (preInterval == 0) {
    preInterval = now;
  }

  float dt = (now - preInterval) / 1000.0f;
  preInterval = now;

  if (dt < 0.0f) dt = 0.0f;

  // Gyro integration + complementary filter
  angleX = filterGyroCoef * (angleX + gyroX * dt) + (1.0f - filterGyroCoef) * angleAccX;
  angleY = filterGyroCoef * (angleY + gyroY * dt) + (1.0f - filterGyroCoef) * angleAccY;
  angleZ = angleZ + gyroZ * dt;

  // Normalize X and Y to keep them stable
  if (angleX > 180.0f) angleX -= 360.0f;
  if (angleX < -180.0f) angleX += 360.0f;

  if (angleY > 180.0f) angleY -= 360.0f;
  if (angleY < -180.0f) angleY += 360.0f;

  if (angleZ > 180.0f) angleZ -= 360.0f;
  if (angleZ < -180.0f) angleZ += 360.0f;
}

void MPU6050::update() {
  fetchData();
}

float MPU6050::getPlane() {
  // Simple tilt magnitude from X/Y angles
  return sqrtf(angleX * angleX + angleY * angleY);
}