#if !defined (AXIS_H)
#define AXIS_H

#include <Arduino.h>
#include <inttypes.h>

enum class AxisesNames
{
    Vx = 0,
    Vy = 1,
    Vz = 2,
    Wx = 3,
    Wy = 4,
    Wz = 5
};

class Axises
{
    int16_t axises_[6];
public:
    Axises();

    Axises(const int16_t* axises);

    int16_t& operator[](size_t index);

    int16_t& operator[](AxisesNames index);

    int16_t operator[](AxisesNames index) const;

    int16_t operator[](size_t index) const;

    Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz);

    int16_t getAxis(AxisesNames axis) const;
	int16_t axis(AxisesNames axis) const;

    int16_t getAxis(size_t axis) const;
	int16_t axis(size_t axis) const;
};

// Преобразует float из диапазона [in_min, in_max] в int16_t [-32768, 32767]
int16_t float_range_to_int16(float value, float in_min, float in_max);

#endif