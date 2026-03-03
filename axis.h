#if !defined (AXIS_H)
#define AXIS_H

#include <Arduino.h>
#include <inttypes.h>



class Axises
{
    int16_t axises_[6];
public:

    enum class Names
    {
        x = 0,
        y = 1,
        z = 2,
        Wx = 3,
        Wy = 4,
        Wz = 5
    };

    Axises();

    Axises(const int16_t* axises);

    int16_t& operator[](size_t index);

    int16_t& operator[](Names index);

    int16_t operator[](Names index) const;

    int16_t operator[](size_t index) const;

    Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz);

    int16_t getAxis(Names axis) const;
	int16_t axis(Names axis) const;

    int16_t getAxis(size_t axis) const;
    int16_t axis(size_t axis) const;

    
};

// Преобразует float из диапазона [in_min; in_max] в int16_t [-32768; 32767]
int16_t float_range_to_int16(float value, float in_min, float in_max);
float int16_to_float_range(int16_t value, float in_min, float in_max);

template<typename T>
inline
T mod(T num, T module)
{
    return num - (num/module)*module;
}
template<typename T>
inline
T math_mod(T num, T module)
{
    if (module == 0) {
        return T{0};
    }
    T res = mod(num, module);
    if (res < 0) {
        res += module;
    }
    return res;
}

#endif