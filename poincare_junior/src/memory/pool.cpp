#include "pool.h"

namespace PoincareJ {

size_t Pool::numberOfTrees() const {
  const Block * currentBlock = firstBlock();
  if (!currentBlock) {
    return 0;
  }
  size_t result = 0;
  while (currentBlock != lastBlock()) {
    currentBlock = Node(currentBlock).nextTree().block();
    result++;
  }
  return result;
}

// Reference Table

uint16_t Pool::ReferenceTable::storeNodeAtIndex(Node node, size_t index) {
  if (index >= m_length) {
    assert(!isFull());
    // Increment first to make firstBlock != nullptr
    m_length++;
  }
  nodeOffsetArray()[index] = static_cast<uint16_t>(node.block() - m_pool->firstBlock());
  // Assertion requires valid firstBlock/lastBlock (so the order matters)
  assert(node.block() >=  m_pool->firstBlock() && node.block() <=  m_pool->lastBlock());
  return index;
}

Node Pool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return Node();
  }
  assert(id < m_length);
  return Node(m_pool->firstBlock() + const_cast<Pool::ReferenceTable *>(this)->nodeOffsetArray()[id]);
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
void Pool::ReferenceTable::log(std::ostream & stream, LogFormat format, bool verbose) const {
  stream << "<" << m_pool->name() << "Pool::References NumberOfStoredNode=\"" << numberOfStoredNodes()  << "\">";
  for (size_t i = 0; i < m_length; i++) {
    stream << "\n  <Reference id: " << identifierForIndex(i) << ">";
    Node tree = Pool::ReferenceTable::nodeForIdentifier(i);
    if (!m_pool->contains(tree.block())) {
      stream << "\n    <Corrupted/>";
    } else if (tree.isUninitialized()) {
      stream << "\n    <Uninialized/>";
    } else {
      tree.log(stream, format == LogFormat::Tree, 2, verbose);
    }
    stream << "\n  </Reference>";
  }
  stream << "\n</" << m_pool->name() << "Pool::References>";
  stream << std::endl;
}

#endif

// Pool

#if POINCARE_MEMORY_TREE_LOG

void Pool::log(std::ostream & stream, LogFormat format, bool verbose) {
  const char * formatName = format == LogFormat::Tree ? "tree" : "flat";
  stream << "<" << name() << "Pool format=\"" << formatName << "\" size=\"" << size() << "\">";
  if (format == LogFormat::Tree) {
    for (const Node tree : trees()) {
      tree.log(stream, true, 1, verbose);
    }
  } else {
    for (const Node tree : allNodes()) {
      tree.log(stream, false, 1, verbose);
    }
  }
  stream << std::endl;
  stream << "<" << name() << "/Pool>";
  stream << std::endl;
}

#endif

Pool::Nodes Pool::allNodes() {
  if (firstBlock() == nullptr) {
    return Nodes(nullptr, 0);
  }
  return Nodes(firstBlock(), lastBlock() - firstBlock());
}

Pool::Trees Pool::trees() {
  if (firstBlock() == nullptr) {
    return Trees(nullptr, 0);
  }
  return Trees(firstBlock(), lastBlock() - firstBlock());
}

}
