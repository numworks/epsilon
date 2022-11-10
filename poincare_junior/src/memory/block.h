#ifndef POINCARE_MEMORY_BLOCK_H
#define POINCARE_MEMORY_BLOCK_H

#define POINCARE_MEMORY_TREE_LOG 1
#if POINCARE_MEMORY_TREE_LOG
#include <ostream>
#endif

#include <stddef.h>
#include <stdint.h>
#include <utils.h>

namespace Poincare {

class Block {

/* A block is a byte-long object containing either a type or some value. */

public:
  constexpr Block(uint8_t content = 0) : m_content(content) {}
  bool operator==(const Block& b) const { return b.m_content == m_content; }
  bool operator!=(const Block& b) { return b.m_content != m_content; }

  // Block Navigation
  constexpr const Block * next() const { return this + 1; }
  constexpr Block * next() { return Utils::DeconstifyPtr(&Block::next, this); }
  constexpr const Block * nextNth(int i) const { return this + i; }
  constexpr Block * nextNth(int i) { return Utils::DeconstifyPtr(&Block::nextNth, this, i); }
  constexpr const Block * previous() const { return this - 1; }
  constexpr Block * previous() { return Utils::DeconstifyPtr(&Block::previous, this); }
  constexpr const Block * previousNth(int i) const { return this - i; }
  constexpr Block * previousNth(int i) { return Utils::DeconstifyPtr(&Block::previousNth, this, i); }

  constexpr explicit operator uint8_t() const { return m_content; }
  constexpr explicit operator int8_t() const { return m_content; }
  constexpr static uint8_t k_maxValue = 0xFF;

protected:
  uint8_t m_content;
};

static_assert(sizeof(Block) == 1);

}

#endif
