#include <poincare/tree_pool.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/helpers.h>
#include <poincare/tree_handle.h>
#include <poincare/test/tree/blob_node.h>
#include <poincare/test/tree/pair_node.h>
#include <string.h>
#include <stdint.h>
#include <poincare_layouts.h>
#include <poincare_nodes.h>

namespace Poincare {

TreePool * TreePool::SharedStaticPool = nullptr;

void TreePool::freeIdentifier(uint16_t identifier) {
  if (TreeNode::IsValidIdentifier(identifier) && identifier < MaxNumberOfNodes) {
    m_nodeForIdentifierOffset[identifier] = UINT16_MAX;
    m_identifiers.push(identifier);
  }
}

void TreePool::move(TreeNode * destination, TreeNode * source, int realNumberOfSourceChildren) {
  size_t moveSize = source->deepSize(realNumberOfSourceChildren);
  moveNodes(destination, source, moveSize);
}

void TreePool::moveChildren(TreeNode * destination, TreeNode * sourceParent) {
  size_t moveSize = sourceParent->deepSize(-1) - Helpers::AlignedSize(sourceParent->size(), ByteAlignment);
  moveNodes(destination, sourceParent->next(), moveSize);
}

void TreePool::removeChildren(TreeNode * node, int nodeNumberOfChildren) {
  for (int i = 0; i < nodeNumberOfChildren; i++) {
    TreeNode * child = node->childAtIndex(0);
    int childNumberOfChildren = child->numberOfChildren();
    /* The new child will be put at the address last(), but removed from its
     * previous position, hence the newAddress we use. */
    TreeNode * newAddress = (TreeNode *)((char *)last() - (char *)child->deepSize(childNumberOfChildren));
    move(last(), child, childNumberOfChildren);
    newAddress->release(newAddress->numberOfChildren());
  }
  node->eraseNumberOfChildren();
}

TreeNode * TreePool::deepCopy(TreeNode * node) {
  size_t size = node->deepSize(-1);
  return copyTreeFromAddress(static_cast<void *>(node), size);
}

TreeNode * TreePool::copyTreeFromAddress(const void * address, size_t size) {
  void * ptr = alloc(size);
  memcpy(ptr, address, size);
  TreeNode * copy = reinterpret_cast<TreeNode *>(ptr);
  renameNode(copy, false);
  for (TreeNode * child : copy->depthFirstChildren()) {
    renameNode(child, false);
    child->retain();
  }
  return copy;
}

void TreePool::removeChildrenAndDestroy(TreeNode * nodeToDestroy, int nodeNumberOfChildren) {
  removeChildren(nodeToDestroy, nodeNumberOfChildren);
  discardTreeNode(nodeToDestroy);
}

void TreePool::moveNodes(TreeNode * destination, TreeNode * source, size_t moveSize) {
  assert(moveSize % 4 == 0);
  assert((((uintptr_t)source) % 4) == 0);
  assert((((uintptr_t)destination) % 4) == 0);

  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = moveSize/4;

  if (Helpers::Rotate(dst, src, len)) {
    updateNodeForIdentifierFromNode(dst < src ? destination : source);
  }
}

#if POINCARE_TREE_LOG
void TreePool::flatLog(std::ostream & stream) {
  size_t size = static_cast<char *>(m_cursor) - static_cast<char *>(buffer());
  stream << "<TreePool format=\"flat\" size=\"" << size << "\">";
  for (TreeNode * node : allNodes()) {
    node->log(stream, false);
  }
  stream << "</TreePool>";
  stream << std::endl;
}

void TreePool::treeLog(std::ostream & stream) {
  stream << "<TreePool format=\"tree\" size=\"" << (int)(m_cursor-buffer()) << "\">";
  for (TreeNode * node : roots()) {
    node->log(stream, true);
  }
  stream << "</TreePool>";
  stream << std::endl;
}

#endif

int TreePool::numberOfNodes() const {
  int count = 0;
  TreeNode * firstNode = first();
  TreeNode * lastNode = last();
  while (firstNode != lastNode) {
    count++;
    firstNode = firstNode->next();
  }
  return count;
}

void * TreePool::alloc(size_t size) {
  size = Helpers::AlignedSize(size, ByteAlignment);
  if (m_cursor + size > buffer() + BufferSize) {
    ExceptionCheckpoint::Raise();
  }
  void * result = m_cursor;
  m_cursor += size;
  return result;
}

void TreePool::dealloc(TreeNode * node, size_t size) {
  size = Helpers::AlignedSize(size, ByteAlignment);
  char * ptr = reinterpret_cast<char *>(node);
  assert(ptr >= buffer() && ptr < m_cursor);

  // Step 1 - Compact the pool
  memmove(
    ptr,
    ptr + size,
    m_cursor - (ptr + size)
  );
  m_cursor -= size;

  // Step 2: Update m_nodeForIdentifierOffset for all nodes downstream
  updateNodeForIdentifierFromNode(node);
}

void TreePool::discardTreeNode(TreeNode * node) {
  uint16_t nodeIdentifier = node->identifier();
  size_t size = node->size();
  node->~TreeNode();
  dealloc(node, size);
  freeIdentifier(nodeIdentifier);
}

void TreePool::registerNode(TreeNode * node) {
  uint16_t nodeID = node->identifier();
  assert(nodeID < MaxNumberOfNodes);
  const int nodeOffset = (((char *)node) - (char *)m_alignedBuffer)/ByteAlignment;
  assert(nodeOffset < k_maxNodeOffset); // Check that the offset can be stored in a uint16_t
  m_nodeForIdentifierOffset[nodeID] = nodeOffset;
}

void TreePool::updateNodeForIdentifierFromNode(TreeNode * node) {
  for (TreeNode * n : Nodes(node)) {
    registerNode(n);
  }
}

void TreePool::freePoolFromNode(TreeNode * firstNodeToDiscard) {
  assert(firstNodeToDiscard != nullptr);
  assert(firstNodeToDiscard >= first());
  assert(firstNodeToDiscard <= last());

  if (firstNodeToDiscard < last()) {
    // There should be no tree that continues into the pool zone to discard
    assert(firstNodeToDiscard->parent() == nullptr);
  }
  TreeNode * currentNode = firstNodeToDiscard;
  TreeNode * lastNode = last();
  while (currentNode < lastNode) {
    freeIdentifier(currentNode->identifier());
    currentNode = currentNode->next();
  }
  m_cursor = reinterpret_cast<char *>(firstNodeToDiscard);
}

}
