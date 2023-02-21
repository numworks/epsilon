#include <poincare/tree_handle.h>
#include <poincare/tree_node.h>
#include <poincare/tree_pool.h>

namespace Poincare {

// Node operations

void TreeNode::release(int currentNumberOfChildren) {
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    deleteParentIdentifierInChildren();
    TreePool::sharedPool->removeChildrenAndDestroy(this,
                                                   currentNumberOfChildren);
  }
}

void TreeNode::rename(uint16_t identifier, bool unregisterPreviousIdentifier,
                      bool skipChildrenUpdate) {
  if (unregisterPreviousIdentifier) {
    /* The previous identifier should not always be unregistered. For instance,
     * if the node is a clone and still has the original node's identifier,
     * unregistering it would lose the access to the original node. */
    TreePool::sharedPool->unregisterNode(this);
  }
  m_identifier = identifier;
  m_referenceCounter = 0;
  TreePool::sharedPool->registerNode(this);
  if (skipChildrenUpdate) {
    return;
  }
  updateParentIdentifierInChildren();
}

// Hierarchy

TreeNode *TreeNode::parent() const {
  assert(m_parentIdentifier != m_identifier);
  return TreeHandle::hasNode(m_parentIdentifier)
             ? TreePool::sharedPool->node(m_parentIdentifier)
             : nullptr;
}

TreeNode *TreeNode::root() {
  TreeNode *result = this;
  TreeNode *resultParent = result->parent();
  while (resultParent != nullptr) {
    result = resultParent;
    resultParent = result->parent();
  }
  return result;
}

int TreeNode::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  TreeNode *nextSiblingNode = nextSibling();
  TreeNode *currentNode = next();
  while (currentNode != nextSiblingNode) {
    result++;
    currentNode = currentNode->next();
  }
  return result;
}

TreeNode *TreeNode::childAtIndex(int i) const {
  assert(i >= 0);
  assert(i < numberOfChildren());
  TreeNode *child = next();
  while (i > 0) {
    child = child->nextSibling();
    assert(child != nullptr);
    i--;
  }
  assert(m_identifier != child->identifier());
  return child;
}

int TreeNode::indexOfChild(const TreeNode *child) const {
  assert(child != nullptr);
  int childrenCount = numberOfChildren();
  TreeNode *childAtIndexi = next();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi == child) {
      return i;
    }
    childAtIndexi = childAtIndexi->nextSibling();
  }
  return -1;
}

int TreeNode::indexInParent() const {
  TreeNode *p = parent();
  if (p == nullptr) {
    return -1;
  }
  return p->indexOfChild(this);
}

bool TreeNode::hasChild(const TreeNode *child) const {
  for (TreeNode *c : directChildren()) {
    if (child == c) {
      return true;
    }
  }
  return false;
}

bool TreeNode::hasAncestor(const TreeNode *node, bool includeSelf) const {
  if (includeSelf && node == this) {
    return true;
  }
  for (TreeNode *t : node->depthFirstChildren()) {
    if (this == t) {
      return true;
    }
  }
  return false;
}

bool TreeNode::hasSibling(const TreeNode *e) const {
  TreeNode *p = parent();
  if (p == nullptr) {
    return false;
  }
  for (TreeNode *childNode : p->directChildren()) {
    if (childNode == e) {
      return true;
    }
  }
  return false;
}

TreeNode *TreeNode::nextSibling() const {
  int remainingNodesToVisit = numberOfChildren();
  TreeNode *node = const_cast<TreeNode *>(this)->next();
  while (remainingNodesToVisit > 0) {
    remainingNodesToVisit += node->numberOfChildren();
    node = node->next();
    remainingNodesToVisit--;
  }
  return node;
}

TreeNode *TreeNode::lastDescendant() const {
  TreeNode *node = const_cast<TreeNode *>(this);
  int remainingNodesToVisit = node->numberOfChildren();
  while (remainingNodesToVisit > 0) {
    node = node->next();
    remainingNodesToVisit--;
    remainingNodesToVisit += node->numberOfChildren();
  }
  return node;
}

// Protected

#if POINCARE_TREE_LOG
void TreeNode::log(std::ostream &stream, bool recursive, int indentation,
                   bool verbose) {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "<";
  logNodeName(stream);
  if (verbose) {
    stream << " id=\"" << m_identifier << "\"";
    stream << " refCount=\"" << (int16_t)m_referenceCounter << "\"";
    stream << " size=\"" << size() << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (TreeNode *child : directChildren()) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child->log(stream, recursive, indentation + 1, verbose);
    }
  }
  if (tagIsClosed) {
    stream << "\n";
    for (int i = 0; i < indentation; ++i) {
      stream << "  ";
    }
    stream << "</";
    logNodeName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}
#endif

size_t TreeNode::deepSize(int realNumberOfChildren) const {
  if (realNumberOfChildren == -1) {
    return reinterpret_cast<char *>(nextSibling()) -
           reinterpret_cast<const char *>(this);
  }
  TreeNode *realNextSibling = next();
  for (int i = 0; i < realNumberOfChildren; i++) {
    realNextSibling = realNextSibling->nextSibling();
  }
  return reinterpret_cast<char *>(realNextSibling) -
         reinterpret_cast<const char *>(this);
}

bool TreeNode::deepIsGhost() const {
  if (isGhost()) {
    return true;
  }
  for (TreeNode *c : directChildren()) {
    if (c->deepIsGhost()) {
      return true;
    }
  }
  return false;
}

void TreeNode::changeParentIdentifierInChildren(uint16_t id) const {
  for (TreeNode *c : directChildren()) {
    c->setParentIdentifier(id);
  }
}

}  // namespace Poincare
