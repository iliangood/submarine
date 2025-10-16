#if !defined (AXIS_H)
#define AXIS_H
#include <utils.h>

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
    int16_t axises[6];
public:
    Axises();

    Axises(const int16_t* axises);

    int16_t& operator[](int index);

    int16_t& operator[](AxisesNames index);

    int16_t operator[](AxisesNames index) const;

    int16_t operator[](int index) const;

    Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz);

    int getAxis(AxisesNames axis) const;

    int getAxis(int axis) const;
};

#endif