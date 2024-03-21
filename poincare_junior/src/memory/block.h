#ifndef POINCARE_MEMORY_BLOCK_H
#define POINCARE_MEMORY_BLOCK_H

#if POINCARE_TREE_LOG
#include <ostream>
#endif

#include <assert.h>
#include <omgpj.h>
#include <stddef.h>
#include <stdint.h>

#include <initializer_list>

namespace PoincareJ {

enum class BlockType : uint8_t;
class Block {
  /* A block is a byte-long object containing either a type or some value. */

 public:
  constexpr Block(uint8_t content = 0) : m_content(content) {}
  constexpr Block(BlockType type) : m_content(static_cast<uint8_t>(type)) {}
  bool operator==(const Block& b) const { return b.m_content == m_content; }
  bool operator!=(const Block& b) { return b.m_content != m_content; }

  // Block Navigation
  constexpr const Block* next() const { return this + 1; }
  constexpr Block* next() { return Utils::DeconstifyPtr(&Block::next, this); }
  constexpr const Block* nextNth(int i) const { return this + i; }
  constexpr Block* nextNth(int i) {
    return Utils::DeconstifyPtr(&Block::nextNth, this, i);
  }

  constexpr explicit operator uint8_t() const { return m_content; }
  constexpr explicit operator int8_t() const { return m_content; }
  constexpr static uint8_t k_maxValue = 0xFF;

 public:
  // Member variables need to be public for the class to be an aggregate
  uint8_t m_content;
};

static_assert(sizeof(Block) == 1);

}  // namespace PoincareJ

#endif
