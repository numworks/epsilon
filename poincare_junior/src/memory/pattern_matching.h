#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <poincare_junior/src/expression/placeholder.h>

#include <array>

#include "edition_reference.h"
#include "k_creator.h"
#include "node.h"
#include "node_iterator.h"
#include "pool.h"

namespace PoincareJ {

namespace PatternMatching {

class Context {
 public:
  Node& operator[](uint8_t tag) { return m_array[tag]; }
  const Node& operator[](uint8_t tag) const { return m_array[tag]; }
  bool isUninitialized() const;

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

 private:
  Node m_array[Placeholder::Tag::numberOfTags];
};

Context Match(const Node pattern, const Node source,
              Context context = Context());
EditionReference Create(const Node structure,
                        const Context context = Context());

};  // namespace PatternMatching

}  // namespace PoincareJ

#endif
