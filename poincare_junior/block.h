#ifndef POINCARE_BLOCK_H
#define POINCARE_BLOCK_H

#define POINCARE_TREE_LOG 1
#if POINCARE_TREE_LOG
#include <ostream>
#endif

#include <stddef.h>
#include <stdint.h>
#include "utils.h"

namespace Poincare {

class Block {

/* A block is a byte-long object containing either a type or some value. */

public:
  constexpr Block(uint8_t content = 0) : m_content(content) {}
  bool operator==(const Block& b) const { return b.m_content == m_content; }
  bool operator!=(const Block& b) { return b.m_content != m_content; }

  // Block Navigation
  const Block * next() const { return this + 1; }
  Block * next() { return Utils::DeconstifyPtr(&Block::next, this); }
  const Block * nextNth(int i) const { return this + i; }
  Block * nextNth(int i) { return Utils::DeconstifyPtr(&Block::nextNth, this, i); }
  const Block * previous() const { return this - 1; }
  Block * previous() { return Utils::DeconstifyPtr(&Block::previous, this); }
  const Block * previousNth(int i) const { return this - i; }
  Block * previousNth(int i) { return Utils::DeconstifyPtr(&Block::previousNth, this, i); }

  constexpr explicit operator uint8_t() const { return m_content; }
protected:
  uint8_t m_content;
};

static_assert(sizeof(Block) == 1);

}

#endif
