#ifndef REGS_REGISTER_H
#define REGS_REGISTER_H

#include <stdint.h>

template <typename T>
class Register {
public:
  Register() = delete;
  void set(T value) volatile {
    m_value = value;
  }
  T get() volatile {
    return m_value;
  }
protected:
  static constexpr T bit_range_mask(uint8_t high, uint8_t low) {
    return ((((T)1)<<(high-low+1))-1)<<low;
  }
  static constexpr T bit_range_value(T value, uint8_t high, uint8_t low) {
    return (value<<low) & bit_range_mask(high,low);
  }
  static constexpr T bit_range_set_value(uint8_t high, uint8_t low, T originalValue, T targetValue) {
    return (originalValue & ~bit_range_mask(high,low))|bit_range_value(targetValue, high, low);
  }
  void set(uint8_t high, uint8_t low, T value) volatile {
    m_value = bit_range_set_value(high, low, m_value, value);
  }
  T get(uint8_t high, uint8_t low) volatile {
    return (m_value & bit_range_mask(high,low)) >> low;
  }
private:
  T m_value;
};

typedef Register<uint16_t> Register16;
typedef Register<uint32_t> Register32;
typedef Register<uint64_t> Register64;

#define REGS_BOOL_FIELD(name,bit) bool get##name() volatile { return get(bit,bit); }; void set##name(bool enable) volatile { set(bit,bit,enable); };
#define REGS_TYPE_FIELD(name,high,low) name get##name() volatile { return (name)get(high,low); }; void set##name(name v) volatile { set(high, low, (uint8_t)v); };
#define REGS_REGISTER_AT(name, offset) volatile name * name() const { return (class name *)(Base() + offset); };

#endif
