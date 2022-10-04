#ifndef POINCARE_VALUE_BLOCK_H
#define POINCARE_VALUE_BLOCK_H

#include "block.h"
#include "type_block.h"

namespace Poincare {

class ValueBlock : public Block {
public:
  constexpr ValueBlock(uint8_t value) : Block(value) {}
  uint8_t value() const { return m_content; }
};

static_assert(sizeof(ValueBlock) == sizeof(Block));

}

#endif
