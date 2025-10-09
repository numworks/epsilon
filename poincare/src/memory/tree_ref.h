#pragma once

#include "k_tree.h"
#include "tree.h"
#include "tree_stack.h"

namespace Poincare::Internal {

class TreeRef {
 public:
  TreeRef() : m_identifier(TreeStack::ReferenceTable::NoNodeIdentifier) {}
  TreeRef(Tree* node);

  template <KTreeConcept T>
  TreeRef(T t) : TreeRef(static_cast<const Tree*>(t)) {}

  TreeRef(Block* blocks) : TreeRef(Tree::FromBlocks(blocks)) {}

  // Copies create a new ref with a new id
  TreeRef(const TreeRef& other) : TreeRef(static_cast<Tree*>(other)) {}

  TreeRef& operator=(const TreeRef& other) {
    *this = TreeRef(static_cast<Tree*>(other));
    return *this;
  }

  TreeRef& operator=(Tree* tree);

  // Moves steal the id
  TreeRef(TreeRef&& other) {
    m_identifier = other.m_identifier;
    other.m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
  }

  TreeRef& operator=(TreeRef&& other) {
    if (m_identifier != other.m_identifier) {
      SharedTreeStack->deleteIdentifier(m_identifier);
      m_identifier = other.m_identifier;
    }
    other.m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
    return *this;
  }

  ~TreeRef() { stopTracking(); }

  void stopTracking() {
    SharedTreeStack->deleteIdentifier(m_identifier);
    m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
  }

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  // Make if (ref) do the same thing as if (treePtr)
  explicit operator bool() const { return tree() != nullptr; }

  operator Tree*() const { return tree(); }
  Tree* operator->() { return tree(); }

  /* Comparison */
  inline bool operator==(const TreeRef& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() && tree() == t.tree());
  }
  inline bool operator!=(const TreeRef& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() || tree() != t.tree());
  }

  bool isUninitialized() const { return tree() == nullptr; }

  uint16_t identifier() const { return m_identifier; }

 private:
  TreeRef(const Tree* tree) : TreeRef(tree->cloneTree()){};
  Tree* tree() const;
  uint16_t m_identifier;
};

inline bool operator==(Tree* n, const TreeRef& r) {
  return n == static_cast<Tree*>(r);
}

}  // namespace Poincare::Internal
