#if !defined (CONFIG_H)
#define CONFIG_H
#include <EEPROM.h>
#include <assert.h>

template<size_t N>
class Config {
  size_t configs_[N];
  size_t currentAddr_ = 0;
  size_t currentIndex_ = 0;
  
public:
  template<typename T>
  void addConfig()
  {
    assert(N > currentIndex_);
    configs_[currentIndex_] = currentAddr_;
    ++currentIndex_;
    currentAddr_ += sizeof(T);
  }
  template<typename T>
  T read(size_t index) {
    T res;
    EEPROM.get(configs_[index], res);
    return res;
  }
  template<typename T>
  void write(size_t index, T value) {
    EEPROM.put(configs_[index], value);
  }


};

#endif