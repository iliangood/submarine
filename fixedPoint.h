#if !defined FIXED_POINT_H
#define FIXED_POINT_H
#include <limits.h>

// Всего есть 16 бит, P означа.т сколько бит будет после запятой для итогового числа, T - тип числа
template<typename T, size_t P>
constexpr T toFixedPoint(double num)
{
  return static_cast<T>(round(num * (1LL << P)));
}

template<typename BT, typename ST, ssize_t P>
class fixedPoint
{
  BT num_;
public:
  constexpr fixedPoint(double num): num_(static_cast<T>(round(num * (1LL << P)))) {}
  constexpr fixedPoint(float num): num_(static_cast<T>(round(num * (1LL << P)))) {}
  constexpr fixedPoint(const fixedPoint<BT, ST, P>& other) : num_(other.num_) {}
  constexpr fixedPoint(const BT& num) : num_(num) {}

  template<typename BTo, typename STo, ssize_t Po>
  static fixedPoint<BT, ST, P> fixedPoint(const fixedPoint<BTo, STo, Po>& other)
  {
    if(sizeof(BT) > sizeof(BTo))
    {
      BT whole = static_cast<BT>(other.num_) >> Po;
      BT fractional = static_cast<BT>(other.num_) ^ (whole << Po);
      return fixedPoint(fractional | (whole << P));
    }
    BTo whole = other.num_ >> Po;
    BTo fractional = other.num_ ^ (whole << Po);
    return fixedPoint(constrain( fractional | (whole << P), numeric_limits<ST>::min(), numeric_limits<ST>::max()));
  }

  constexpr double toDouble()
  {
    static_cast<double>(num_)/(1 << P);
  }
  
  constexpr float toFloat()
  {
    static_cast<float>(num_)/(1 << P);
  }

  template<ssize_t Po>
  fixedPoint<BT, ST, min(P, Po)> operator-(const fixedPoint<BT, ST, Po>& other)
  {
    constexpr ssize_t min_p = min(P, Po);
    return fixedPoint(num_ >> (P - min_p)) - (other.num_ >> (P - min_p)));
  }

  template<ssize_t Po>
  fixedPoint<BT, ST, min(P, Po)> operator+(const fixedPoint<BT, ST, Po>& other)
  {
    constexpr ssize_t min_p = min(P, Po);
    return fixedPoint((num_ >> (P - min_p)) - (other.num_ >> (P - min_p)));
  }

  template<ssize_t Po>
  fixedPoint<BT, ST, min(P, Po)> operator/(const fixedPoint<BT, ST, Po>& other)
  {
    constexpr ssize_t min_p = min(P, Po);
    return fixedPoint((num_ >> (P - min_p)) / (other.num_ >> (P - min_p)) << min_p);
  }

  template<ssize_t Po>
  fixedPoint<BT, ST, min(P, Po)> operator*(const fixedPoint<BT, ST, Po>& other)
  {
    constexpr ssize_t min_p = min(P, Po);
    return  fixedPoint(static_cast<BT>((static_cast<ST>(num_) >> (P - min_p)) * (static_cast<ST>(other.num_) >> (P - min_p)) >> min_p));
  }


}
#endif