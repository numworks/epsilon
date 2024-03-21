#include "pool.h"

#include <poincare_junior/include/poincare.h>

namespace PoincareJ {

size_t Pool::numberOfTrees() const {
  const Block* currentBlock = firstBlock();
  size_t result = 0;
  while (currentBlock < lastBlock()) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
    result++;
  }
  assert(currentBlock == lastBlock());
  return result;
}

bool Pool::isRootBlock(const Block* block, bool allowLast) const {
  const Block* currentBlock = firstBlock();
  if (block >= lastBlock()) {
    return allowLast && block == lastBlock();
  }
  while (currentBlock < block) {
    currentBlock = Tree::FromBlocks(currentBlock)->nextTree()->block();
  }
  return currentBlock == block;
}

// Reference Table

uint16_t Pool::ReferenceTable::storeNodeAtIndex(Tree* node, size_t index) {
  if (index >= m_length) {
    assert(index == m_length);
    assert(!isFull());
    // Increment first to make firstBlock != nullptr
    m_length++;
  }
  nodeOffsetArray()[index] = static_cast<uint16_t>(
      static_cast<Block*>(node->block()) - m_pool->referenceBlock());
  // Assertion requires valid firstBlock/lastBlock (so the order matters)
  assert(m_pool->contains(node->block()) ||
         node->block() == m_pool->lastBlock());
  return index;
}

Tree* Pool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return nullptr;
  }
  assert(id < m_length);
  uint16_t offset =
      const_cast<Pool::ReferenceTable*>(this)->nodeOffsetArray()[id];
  if (offset == InvalidatedOffset) {
    return nullptr;
  }
  return Tree::FromBlocks(m_pool->referenceBlock() + offset);
}

bool Pool::ReferenceTable::reset() {
  if (m_length == 0) {
    // We can't reset an empty table
    return false;
  }
  m_length = 0;
  return true;
}

#if POINCARE_TREE_LOG

void Pool::ReferenceTable::logIdsForNode(std::ostream& stream,
                                         const Tree* node) const {
  bool found = false;
  for (size_t i = 0; i < m_length; i++) {
    Tree* n = Pool::ReferenceTable::nodeForIdentifier(i);
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

#if POINCARE_TREE_LOG

void Pool::logNode(std::ostream& stream, const Tree* node, bool recursive,
                   bool verbose, int indentation) {
  Indent(stream, indentation);
  stream << "<Reference id=\"";
  referenceTable()->logIdsForNode(stream, node);
  stream << "\">\n";
  node->log(stream, recursive, verbose, indentation + 1);
  Indent(stream, indentation);
  stream << "</Reference>" << std::endl;
}

void Pool::log(std::ostream& stream, LogFormat format, bool verbose,
               int indentation) {
  const char* formatName = format == LogFormat::Tree ? "tree" : "flat";
  Indent(stream, indentation);
  stream << "<" << name() << "Pool format=\"" << formatName << "\" size=\""
         << size() << "\">\n";
  if (format == LogFormat::Tree) {
    for (const Tree* tree : trees()) {
      logNode(stream, tree, true, verbose, indentation + 1);
    }
  } else {
    for (const Tree* tree : allNodes()) {
      logNode(stream, tree, false, verbose, indentation + 1);
    }
  }
  Indent(stream, indentation);
  stream << "</" << name() << "Pool>" << std::endl;
}

#endif

}  // namespace PoincareJ
