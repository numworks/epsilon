#include "pool.h"

#include <poincare_junior/include/poincare.h>

namespace PoincareJ {

size_t Pool::numberOfTrees() const {
  const TypeBlock *currentBlock = firstBlock();
  size_t result = 0;
  while (currentBlock < lastBlock()) {
    currentBlock = Node(currentBlock).nextTree().block();
    result++;
  }
  assert(currentBlock == lastBlock());
  return result;
}

// Reference Table

uint16_t Pool::ReferenceTable::storeNodeAtIndex(Node node, size_t index) {
  if (index >= m_length) {
    assert(!isFull());
    // Increment first to make firstBlock != nullptr
    m_length++;
  }
  nodeOffsetArray()[index] =
      static_cast<uint16_t>(node.block() - m_pool->firstBlock());
  // Assertion requires valid firstBlock/lastBlock (so the order matters)
  assert(m_pool->contains(node.block()) || node.block() == m_pool->lastBlock());
  return index;
}

Node Pool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return Node();
  }
  assert(id < m_length);
  uint16_t offset =
      const_cast<Pool::ReferenceTable *>(this)->nodeOffsetArray()[id];
  if (offset == NoNodeIdentifier) {
    return Node();
  }
  assert(offset <= m_pool->size());
  return Node(m_pool->firstBlock() + offset);
}

bool Pool::ReferenceTable::reset() {
  if (m_length == 0) {
    // We can't reset an empty table
    return false;
  }
  m_length = 0;
  return true;
}

#if POINCARE_MEMORY_TREE_LOG

void Pool::ReferenceTable::logIdsForNode(std::ostream &stream,
                                         Node node) const {
  bool found = false;
  for (size_t i = 0; i < m_length; i++) {
    Node n = Pool::ReferenceTable::nodeForIdentifier(i);
    if (node == n) {
      stream << identifierForIndex(i) << ", ";
      found = true;
    }
  }
  if (found == false) {
    stream << "ø";
  }
}

#endif

// Pool

#if POINCARE_MEMORY_TREE_LOG

void Pool::logNode(std::ostream &stream, Node node, bool recursive,
                   bool verbose, int indentation) {
  Indent(stream, indentation);
  stream << "<Reference id=\"";
  referenceTable()->logIdsForNode(stream, node);
  stream << "\">";
  node.log(stream, recursive, verbose, indentation + 1);
  stream << std::endl;
  Indent(stream, indentation);
  stream << "</Reference>" << std::endl;
}

void Pool::log(std::ostream &stream, LogFormat format, bool verbose,
               int indentation) {
  const char *formatName = format == LogFormat::Tree ? "tree" : "flat";
  Indent(stream, indentation);
  stream << "<" << name() << "Pool format=\"" << formatName << "\" size=\""
         << size() << "\">" << '\n';
  if (format == LogFormat::Tree) {
    for (const Node tree : trees()) {
      logNode(stream, tree, true, verbose, indentation + 1);
    }
  } else {
    for (const Node tree : allNodes()) {
      logNode(stream, tree, false, verbose, indentation + 1);
    }
  }
  Indent(stream, indentation);
  stream << "</" << name() << "Pool>" << std::endl;
}

#endif

}  // namespace PoincareJ
