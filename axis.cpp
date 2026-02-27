#include "axis.h"
#include <utils.h>


Axises::Axises() : axises_{0, 0, 0, 0, 0, 0} {}

Axises::Axises(const int16_t* axises)
{
    for (int i = 0; i < 6; ++i)
    {
        this->axises_[i] = axises[i];
    }
}

int16_t& Axises::operator[](size_t index)
{
    return axises_[index];
}

int16_t& Axises::operator[](Names index)
{
    return axises_[static_cast<int>(index)];
}

int16_t Axises::operator[](Names index) const
{
    return axises_[static_cast<int>(index)];
}

int16_t Axises::operator[](size_t index) const
{
    return axises_[index];
}

Axises::Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz)
{
    (*this)[Names::x] = Vx;
    (*this)[Names::y] = Vy;
    (*this)[Names::z] = Vz;
    (*this)[Names::Wx] = Wx;
    (*this)[Names::Wy] = Wy;
    (*this)[Names::Wz] = Wz;
}

int16_t Axises::getAxis(Names axis) const
{
    return axises_[static_cast<int>(axis)];
}

int16_t Axises::axis(Names axis) const
{
    return axises_[static_cast<int>(axis)];
}

int16_t Axises::getAxis(size_t axis) const
{
    return axises_[axis];
}

int16_t Axises::axis(size_t axis) const
{
    return axises_[axis];
}


int16_t float_range_to_int16(float value, float in_min, float in_max)
{
    value = clamp(value, in_min, in_max);

    float normalized = (value - in_min) / (in_max - in_min) * 2.0f - 1.0f;

    float scaled = normalized * 32767.0f;

    int32_t rounded;
    if (scaled >= 0.0f)
        rounded = static_cast<int32_t>(scaled + 0.5f);
    else
        rounded = static_cast<int32_t>(scaled - 0.5f);

    if (rounded > 32767) rounded = 32767;
    if (rounded < -32768) rounded = -32768;

    return static_cast<int16_t>(rounded);
}