#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

extern "C" {
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
}

#include <array>

namespace Ion {
namespace Keyboard {

constexpr int NumberOfKeys = ION_KEYBOARD_ROWS * ION_KEYBOARD_COLUMNS;

#define PLAIN_KEY(...) KEY(__VA_ARGS__)
#define SHIFT_KEY(...) KEY(__VA_ARGS__)
#define ALPHA_KEY(...) KEY(__VA_ARGS__)
#define SHIFT_ALPHA_KEY(...) KEY(__VA_ARGS__)
#define DUMMY_KEY(...) KEY(__VA_ARGS__)

enum class Key {
#define KEY(N, R, C, ...) N = R * ION_KEYBOARD_COLUMNS + C,
#include <ion/keys.inc>
#undef KEY
  None = NumberOfKeys,
};

#undef DUMMY_KEY
#define DUMMY_KEY(...)

constexpr Key ValidKeys[] = {
#define KEY(N, ...) Key::N,
#include <ion/keys.inc>
#undef KEY
};

#undef PLAIN_KEY
#undef SHIFT_KEY
#undef ALPHA_KEY
#undef SHIFT_ALPHA_KEY
#undef DUMMY_KEY

constexpr int NumberOfValidKeys = std::size(ValidKeys);

class State {
 public:
  constexpr State(uint64_t s = 0) : m_bitField(s) {}
  /* "Shift behavior is undefined if the right operand is negative, or greater
   * than or equal to the length in bits of the promoted left operand" according
   * to C++ spec but k is always in [0:52]. */
  explicit constexpr State(Key k) : m_bitField((uint64_t)1 << (uint8_t)k) {}
  inline bool keyDown(Key k) const {
    assert((uint8_t)k < 64);
    return (m_bitField >> (uint8_t)k) & 1;
  }
  operator uint64_t() const { return m_bitField; }
  void setKey(Key k) {
    if (k != Key::None) {
      m_bitField |= (uint64_t)1 << (uint8_t)k;
    }
  }

 private:
  uint64_t m_bitField;
};

State scan(bool forPython = false);
State scanForInterruptionAndPopState();
State popState();

static_assert(sizeof(State) * 8 > NumberOfKeys,
              "Ion::Keyboard::State cannot hold a keyboard snapshot");

}  // namespace Keyboard
}  // namespace Ion

#endif
