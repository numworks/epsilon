#ifndef POINCARE_MEMORY_VALUE_BLOCK_H
#define POINCARE_MEMORY_VALUE_BLOCK_H

#include "block.h"
#include "type_block.h"

namespace Poincare {

class ValueBlock : public Block {
public:
  constexpr ValueBlock(uint8_t value) : Block(value) {}
};

static_assert(sizeof(ValueBlock) == sizeof(Block));

}

#endif
