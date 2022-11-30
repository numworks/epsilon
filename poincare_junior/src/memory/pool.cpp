#include "pool.h"

namespace Poincare {

// Reference Table

uint16_t Pool::ReferenceTable::storeNode(Node node) {
  assert(!isFull());
  // Increment first to make firstBlock != nullptr
  m_length++;
  m_nodeOffsetForIdentifier[m_length - 1] = static_cast<uint16_t>(node.block() - m_pool->firstBlock());
  return m_length - 1;
}

Node Pool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return Node();
  }
  assert(id < m_length);
  return Node(m_pool->firstBlock() + m_nodeOffsetForIdentifier[id]);
}

bool Pool::ReferenceTable::reset() {
  if (m_length == 0) {
    // We can't reset an empty table
    return false;
  }
  m_length = 0;
  return true;
}

// Pool

#if POINCARE_MEMORY_TREE_LOG
void Pool::flatLog(std::ostream & stream) {
  stream << "<Pool format=\"flat\" size=\"" << size()  << "\">";
  for (const Node node : allNodes()) {
    node.log(stream, false);
  }
  stream << "</Pool>";
  stream << std::endl;
}

void Pool::treeLog(std::ostream & stream, bool verbose) {
  stream << "<Pool format=\"tree\" size=\"" << size() << "\">";
  for (const Node tree : trees()) {
    tree.log(stream, true, 1, verbose);
  }
  stream << std::endl;
  stream << "</Pool>";
  stream << std::endl;
}

#endif

Pool::Nodes Pool::allNodes() {
  if (firstBlock() == nullptr) {
    return Nodes(nullptr, 0);
  }
  return Nodes(firstBlock(), lastBlock() - static_cast<Block *>(firstBlock()));
}

Pool::Trees Pool::trees() {
  if (firstBlock() == nullptr) {
    return Trees(nullptr, 0);
  }
  return Trees(firstBlock(), lastBlock() - static_cast<Block *>(firstBlock()));
}

}
