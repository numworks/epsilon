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

  /* Comparison */
  inline bool operator==(const EditionReference& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() && node() == t.node());
  }
  inline bool operator!=(const EditionReference& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() || node() != t.node());
  }

  EditionReference clone() {
    return EditionPool::sharedEditionPool()->clone(*this);
  }

  operator Node*() const { return node(); }
  bool isUninitialized() const { return node() == nullptr; }
  TypeBlock* block() { return node()->block(); }
  BlockType type() const { return node()->type(); }
  size_t treeSize() const { return node()->treeSize(); }

  uint16_t identifier() const { return m_identifier; }

  /* Hierarchy */
  Node* nextNode() { return node()->nextNode(); }
  Node* nextTree() { return node()->nextTree(); }
  bool hasChild(EditionReference t) const { return node()->hasChild(t); }
  bool hasAncestor(EditionReference t, bool includeSelf) const {
    return node()->hasAncestor(t, includeSelf);
  }
  int numberOfChildren() const { return node()->numberOfChildren(); }
  int indexOfChild(EditionReference t) const { return node()->indexOfChild(t); }
  Node* childAtIndex(int i) const { return node()->childAtIndex(i); }
  int numberOfDescendants(bool includeSelf) const {
    return node()->numberOfDescendants(includeSelf);
  }
  Node::ConstTreeRange children() { return node()->children(); }

  /* Edition operations on Node* */
  void moveNodeAfterNode(Node* n) { node()->moveNodeAfterNode(n); }
  void moveTreeAfterNode(Node* n) { node()->moveTreeAfterNode(n); }
  void moveNodeBeforeNode(Node* n) { node()->moveNodeBeforeNode(n); }
  void moveTreeBeforeNode(Node* n) { node()->moveTreeBeforeNode(n); }
  void cloneNodeAfterNode(const Node* n) { node()->cloneNodeAfterNode(n); }
  void cloneTreeAfterNode(const Node* n) { node()->cloneTreeAfterNode(n); }
  void cloneNodeBeforeNode(const Node* n) { node()->cloneNodeBeforeNode(n); }
  void cloneTreeBeforeNode(const Node* n) { node()->cloneTreeBeforeNode(n); }
  Node* moveNodeOverNode(Node* n) { return node()->moveNodeOverNode(n); }
  Node* moveTreeOverNode(Node* n) { return node()->moveTreeOverNode(n); }
  Node* moveNodeOverTree(Node* n) { return node()->moveNodeOverTree(n); }
  Node* moveTreeOverTree(Node* n) { return node()->moveTreeOverTree(n); }
  Node* cloneNodeOverNode(const Node* n) {
    return node()->cloneNodeOverNode(n);
  }
  Node* cloneTreeOverNode(const Node* n) {
    return node()->cloneTreeOverNode(n);
  }
  Node* cloneNodeOverTree(const Node* n) {
    return node()->cloneNodeOverTree(n);
  }
  Node* cloneTreeOverTree(const Node* n) {
    return node()->cloneTreeOverTree(n);
  }
  void removeNode() { node()->removeNode(); }
  void removeTree() { node()->removeTree(); }
  void detachNode() { node()->detachNode(); }
  void detachTree() { node()->detachTree(); }

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

void CloneNodeBeforeNode(EditionReference* target, const Node* nodeToClone);
void CloneTreeBeforeNode(EditionReference* target, const Node* treeToClone);

void MoveNodeBeforeNode(EditionReference* target, Node* nodeToMove);
void MoveTreeBeforeNode(EditionReference* target, Node* treeToMove);

inline void MoveNodeOverTree(EditionReference* u, Node* n) {
  *u = u->moveNodeOverTree(n);
}

inline void MoveTreeOverTree(EditionReference* u, Node* n) {
  *u = u->moveTreeOverTree(n);
}

inline void MoveNodeOverNode(EditionReference* u, Node* n) {
  *u = u->moveNodeOverNode(n);
}

inline void MoveTreeOverNode(EditionReference* u, Node* n) {
  *u = u->moveTreeOverNode(n);
}

inline void CloneTreeOverNode(EditionReference* u, const Node* n) {
  *u = u->cloneTreeOverNode(n);
}

inline void CloneTreeOverTree(EditionReference* u, const Node* n) {
  *u = u->cloneTreeOverTree(n);
}

inline void CloneNodeOverNode(EditionReference* u, const Node* n) {
  *u = u->cloneNodeOverNode(n);
}

inline void CloneNodeOverTree(EditionReference* u, const Node* n) {
  *u = u->cloneNodeOverTree(n);
}

void SwapTrees(EditionReference* u, EditionReference* v);

}  // namespace PoincareJ

#endif
