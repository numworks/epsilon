#ifndef POINCARE_MEMORY_CONTEXT_H
#define POINCARE_MEMORY_CONTEXT_H

#include <array>
#include "node.h"
#include "node_iterator.h"
#include "edition_reference.h"
#include "pool.h"

namespace PoincareJ {

class Context {
public:
  Node& operator[](Placeholder placeholder) {
    return m_array[static_cast<uint8_t>(placeholder)];
  }

  EditionReference build(Node structure);

  static Context Match(const Node patternTree, Node sourceNode);

private:
  std::array<Node, k_numberOfPlaceholder> m_array;
};

} // namespace PoincareJ

#endif
