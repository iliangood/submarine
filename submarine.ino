enum AxisesNames
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
    Axises() : axises{0, 0, 0, 0, 0, 0} {}

    Axises(const int16_t* axises)
    {
        for (char i = 0; i < 6; ++i)
        {
            this->axises[i] = axises[i];
        }
    }

    int16_t& operator[](int index)
    {
        return axises[index];
    }

    int16_t& operator[](AxisesNames index)
    {
        return axises[static_cast<int>(index)];
    }

    int16_t operator[](AxisesNames index) const
    {
        return axises[static_cast<int>(index)];
    }

    Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz)
    {
        axises[AxisesNames::Vx] = Vx;
        axises[AxisesNames::Vy] = Vy;
        axises[AxisesNames::Vz] = Vz;
        axises[AxisesNames::Wx] = Wx;
        axises[AxisesNames::Wy] = Wy;
        axises[AxisesNames::Wz] = Wz;
    }

    int getAxis(AxisesNames axis) const
    {
        return axises[static_cast<int>(axis)];
    }

    int getAxis(int axis) const
    {
        return axises[axis];
    }
};

class motor
{
  char pinForward;
  char pinBackward;
  int32_t power;
  Axises axises;
public:
  motor(Axises axises, char pinForward, char pinBackward)
  {
    pinMode(pinForward, OUTPUT);
    pinMode(pinBackward, OUTPUT);
    this->axises = axises;
    this->pinBackward = pinBackward;
    this->pinForward = pinForward;
    power = 0;
  }

  int32_t getPower()
  {
    return power;
  }

  int32_t setPower(int32_t power)
  {
    power = max(min(power, 256), -256);
    if(power > 0)
    {
      analogWrite(pinForward, power);
      analogWrite(pinBackward, 0);
    }
    else
    {
      analogWrite(pinForward, 01);
      analogWrite(pinBackward, power);
    }
    return power;
  }

};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
