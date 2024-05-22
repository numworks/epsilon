#ifndef POINCARE_MEMORY_VALUE_BLOCK_H
#define POINCARE_MEMORY_VALUE_BLOCK_H

#include "block.h"
#include "type_block.h"

namespace Poincare::Internal {

class ValueBlock : public Block {
 public:
  constexpr ValueBlock(uint8_t value) : Block(value) {}

  template <typename T>
  T get() const;

  template <typename T>
    requires(sizeof(T) == 1)
  T get() const {
    return *reinterpret_cast<const T*>(this);
  }

  template <typename T>
  void set(T value);

  template <typename T>
    requires(sizeof(T) == 1)
  void set(T value) {
    *reinterpret_cast<T*>(this) = value;
  }

  bool getBit(int index) const {
    assert(index < 8);
    return get<uint8_t>() & (1 << index);
  }

  void setBit(int index, bool value) {
    assert(index < 8);
    uint8_t old = get<uint8_t>() & ~(1 << index);
    if (value) {
      old |= 1 << index;
    }
    set<uint8_t>(old);
  }
};

static_assert(sizeof(ValueBlock) == sizeof(Block));

}  // namespace Poincare::Internal

#endif
