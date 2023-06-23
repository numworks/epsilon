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

  uint16_t identifier() const { return m_identifier; }

  /* Hierarchy */
  Node* nextNode() { return node()->nextNode(); }
  Node* nextTree() { return node()->nextTree(); }
  Node* previousNode() { return node()->previousNode(); }
  Node* previousTree() { return node()->previousTree(); }
  bool hasChild(EditionReference t) const { return node()->hasChild(t); }
  bool hasSibling(EditionReference t) const { return node()->hasSibling(t); }
  bool hasAncestor(EditionReference t, bool includeSelf) const {
    return node()->hasAncestor(t, includeSelf);
  }
  int numberOfChildren() const { return node()->numberOfChildren(); }
  int indexOfChild(EditionReference t) const { return node()->indexOfChild(t); }
  Node* parent() const { return node()->parent(); }
  Node* childAtIndex(int i) const { return node()->childAtIndex(i); }
  int numberOfDescendants(bool includeSelf) const {
    return node()->numberOfDescendants(includeSelf);
  }

  /* Edition operations on Node* */
  void moveNodeAfterNode(Node* n) { moveAt(n, false, false); }
  void moveTreeAfterNode(Node* n) { moveAt(n, false, true); }
  void moveNodeBeforeNode(Node* n) { moveAt(n, true, false); }
  void moveTreeBeforeNode(Node* n) { moveAt(n, true, true); }
  void cloneNodeAfterNode(const Node* n) { cloneAt(n, false, false); }
  void cloneTreeAfterNode(const Node* n) { cloneAt(n, false, true); }
  void cloneNodeBeforeNode(const Node* n) { cloneAt(n, true, false); }
  void cloneTreeBeforeNode(const Node* n) { cloneAt(n, true, true); }
  Node* moveNodeOverNode(Node* n) { return moveOver(n, false, false); }
  Node* moveTreeOverNode(Node* n) { return moveOver(n, false, true); }
  Node* moveNodeOverTree(Node* n) { return moveOver(n, true, false); }
  Node* moveTreeOverTree(Node* n) { return moveOver(n, true, true); }
  Node* cloneNodeOverNode(const Node* n) { return cloneOver(n, false, false); }
  Node* cloneTreeOverNode(const Node* n) { return cloneOver(n, false, true); }
  Node* cloneNodeOverTree(const Node* n) { return cloneOver(n, true, false); }
  Node* cloneTreeOverTree(const Node* n) { return cloneOver(n, true, true); }
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }
  void detachNode() { detach(false); }
  void detachTree() { detach(true); }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

  EditionReference matchAndCreate(const Node* pattern,
                                  const Node* structure) const;
  // Return true if reference has been replaced.
  bool matchAndReplace(const Node* pattern, const Node* structure);

 private:
  Node* node() const;
  void cloneAt(const Node* nodeToClone, bool before, bool newIsTree);
  void moveAt(Node* nodeToMove, bool before, bool newIsTree);
  Node* cloneOver(const Node* n, bool oldIsTree, bool newIsTree);
  Node* moveOver(Node* n, bool oldIsTree, bool newIsTree);
  void detach(bool isTree);
  void remove(bool isTree);
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
