/* This file is to be included as-is by the headers defining peripherals
 * registers; it does not need to be modified.
 * Headers generated from an SVD file by generate_peripherals.py expect to find
 * this file one directory higher. */

#ifndef REGISTER_H
#define REGISTER_H

#include <stddef.h>
#include <stdint.h>

/* Base class for defining peripherals registers.
 * e.g. class REG : public Register<uint32_t> {...}; defines a 32-bits
 * register. */

template <typename S>
class Register {
 public:
  using Size = S;

  Size getBits(int low, int width) const {
    return (m_bits & Bitmask(low, width)) >> low;
  }
  Size setBits(int low, int width, Size bits) {
    m_bits =
        ((bits << low) & Bitmask(low, width)) | (m_bits & ~Bitmask(low, width));
    return m_bits;
  }

 protected:
  Register(Size bits) : m_bits(bits) {}

  void write(size_t address) const { *Pointer(address) = m_bits; }

  constexpr static volatile Size* Pointer(size_t address) {
    return reinterpret_cast<volatile Size*>(address);
  }

 private:
  constexpr static Size Bitmask(int low, int width) {
    constexpr Size one = static_cast<Size>(1);
    return ((one << width) - one) << low;
  }

  Size m_bits;
};

/* Use these macros to define the address, reset value and fiels of a register.
 * These are provided in case a register is not described in the SVD file and
 * needs to be written manually. */

#define REGISTER_BASICS(name, address, reset_value)  \
 private:                                            \
  using Register::Register;                          \
  using Self = name;                                 \
                                                     \
 public:                                             \
  constexpr static size_t k_address = address;       \
  constexpr static Size k_resetValue = reset_value;  \
  static Self Read() { return *Pointer(k_address); } \
  static Self Reset() { return k_resetValue; }       \
  static Self New(Size bits) { return bits; }        \
  void write() const { Register::write(k_address); }

#define FIELD_RO(name, low, width) \
 public:                           \
  Size get##name() { return getBits(low, width); }

#define FIELD_WO(name, low, width) \
 public:                           \
  Self set##name(Size bits) { return setBits(low, width, bits); }

#define FIELD_RW(name, low, width) \
  FIELD_RO(name, low, width);      \
  FIELD_WO(name, low, width);

#endif
