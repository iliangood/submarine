#if !defined PID_H
#define PID_H

class PID
{
  int16_t p_;
  int16_t i_;
  int16_t d_;
  int16_t current_;
  int16_t prev_;
  int16_t target_;
public:
  PID( int16_t p, int16_t i, int16_t d) : p_(p), i_(i), d_(d), current_(0), prev_(0), target_(0) {}
  setTarget(int16_t target);
  void update();
};

#endif