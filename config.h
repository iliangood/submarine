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
  T read(size_t index) const {
    assert(N > index);
    T res;
    EEPROM.get(configs_[index], res);
    return res;
  }
  template<typename T>
  void write(size_t index, T value) {
    assert(N > index);
    EEPROM.put(configs_[index], value);
  }

  friend class ConfigRef;
};

class ConfigRef {
  size_t* configs_;
  const size_t size_;
  size_t* currentAddr_;
  size_t* currentIndex_;
public:
  template<size_t N>
  ConfigRef(Config<N>* config) : configs_(config.configs_), size_(N), currentAddr_(&config.currentAddr_), currentIndex_(&config.currentIndex_){}

  template<typename T>
  void addConfig()
  {
    assert(size_ > *currentIndex_);
    configs_[*currentIndex_] = *currentAddr_;
    ++*currentIndex_;
    *currentAddr_ += sizeof(T);
  }

  template<typename T>
  T read(size_t index) const {
    assert(size_ > index);
    T res;
    EEPROM.get(configs_[index], res);
    return res;
  }

  template<typename T>
  void write(size_t index, T value) {
    assert(size_ > index);
    EEPROM.put(configs_[index], value);
  }
};

class ConstConfigRef {
  const size_t* configs_;
  const size_t size_;
public:
  template<size_t N>
  ConstConfigRef(const Config<N>* config) : configs_(config.configs_), size_(N){}

  template<typename T>
  T read(size_t index) const {
    assert(size_ > index);
    T res;
    EEPROM.get(configs_[index], res);
    return res;
  }
};

#endif