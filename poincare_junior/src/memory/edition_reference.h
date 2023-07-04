#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "edition_pool.h"
#include "node.h"

namespace PoincareJ {

class EditionReference {
 public:
  EditionReference()
      : m_identifier(EditionPool::ReferenceTable::NoNodeIdentifier) {}
  EditionReference(Node* node);
  EditionReference(const Node* node);
  EditionReference(TypeBlock* blocks)
      : EditionReference(Node::FromBlocks(blocks)) {}

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  Node* operator->() { return node(); }

  /* Comparison */
  inline bool operator==(const EditionReference& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() && node() == t.node());
  }
  inline bool operator!=(const EditionReference& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() || node() != t.node());
  }

  operator Node*() const { return node(); }
  bool isUninitialized() const { return node() == nullptr; }

  uint16_t identifier() const { return m_identifier; }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

  EditionReference matchAndCreate(const Node* pattern,
                                  const Node* structure) const;
  // Return true if reference has been replaced.
  bool matchAndReplace(const Node* pattern, const Node* structure);

 private:
  Node* node() const;
  uint16_t m_identifier;
};

void CloneNodeBeforeNode(EditionReference& target, const Node* nodeToClone);
void CloneTreeBeforeNode(EditionReference& target, const Node* treeToClone);

void MoveNodeBeforeNode(EditionReference& target, Node* nodeToMove);
void MoveTreeBeforeNode(EditionReference& target, Node* treeToMove);

inline void MoveNodeOverTree(EditionReference& u, Node* n) {
  u = u->moveNodeOverTree(n);
}

inline void MoveTreeOverTree(EditionReference& u, Node* n) {
  u = u->moveTreeOverTree(n);
}

inline void MoveNodeOverNode(EditionReference& u, Node* n) {
  u = u->moveNodeOverNode(n);
}

inline void MoveTreeOverNode(EditionReference& u, Node* n) {
  u = u->moveTreeOverNode(n);
}

inline void CloneTreeOverNode(EditionReference& u, const Node* n) {
  u = u->cloneTreeOverNode(n);
}

inline void CloneTreeOverTree(EditionReference& u, const Node* n) {
  u = u->cloneTreeOverTree(n);
}

inline void CloneNodeOverNode(EditionReference& u, const Node* n) {
  u = u->cloneNodeOverNode(n);
}

inline void CloneNodeOverTree(EditionReference& u, const Node* n) {
  u = u->cloneNodeOverTree(n);
}

void SwapTrees(EditionReference& u, EditionReference& v);

}  // namespace PoincareJ

#endif
