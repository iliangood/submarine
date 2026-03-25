#if !defined (CONFIG_H)
#define CONFIG_H
template<>
class config {
  size_t current_;
public:
  template<typename T>
  T get() {
    T res = *reinterpeter_cast<*T>(current);
    current += sizeof(T);
    return res;
  }

};
#endif