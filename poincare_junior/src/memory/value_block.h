#ifndef POINCARE_MEMORY_VALUE_BLOCK_H
#define POINCARE_MEMORY_VALUE_BLOCK_H

#include "block.h"
#include "type_block.h"

namespace PoincareJ {

class ValueBlock : public Block {
 public:
  constexpr ValueBlock(uint8_t value) : Block(value) {}

  template <typename T>
  T get() const;

  template <typename T>
    requires(sizeof(T) == 1)
  T get() const {
    return *reinterpret_cast<T*>(this);
  }
};

static_assert(sizeof(ValueBlock) == sizeof(Block));

}  // namespace PoincareJ

#endif
