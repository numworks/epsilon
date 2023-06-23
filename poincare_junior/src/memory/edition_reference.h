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
  void moveNodeAfterNode(Node* nodeToInsert) {
    insert(nodeToInsert, false, false);
  }
  void moveTreeAfterNode(Node* nodeToInsert) {
    insert(nodeToInsert, false, true);
  }
  void moveNodeBeforeNode(Node* nodeToInsert) {
    insert(nodeToInsert, true, false);
  }
  void moveTreeBeforeNode(Node* nodeToInsert) {
    insert(nodeToInsert, true, true);
  }
  void cloneNodeAfterNode(const Node* nodeToClone) {
    insert(nodeToClone, false, false);
  }
  void cloneTreeAfterNode(const Node* nodeToClone) {
    insert(nodeToClone, false, true);
  }
  void cloneNodeBeforeNode(const Node* nodeToClone) {
    insert(nodeToClone, true, false);
  }
  void cloneTreeBeforeNode(const Node* nodeToClone) {
    insert(nodeToClone, true, true);
  }
  Node* replaceNodeByNode(Node* n) { return replaceBy(n, false, false); }
  Node* replaceNodeByTree(Node* n) { return replaceBy(n, false, true); }
  Node* replaceTreeByNode(Node* n) { return replaceBy(n, true, false); }
  Node* replaceTreeByTree(Node* n) { return replaceBy(n, true, true); }
  Node* cloneNodeOverNode(const Node* n) { return replaceBy(n, false, false); }
  Node* cloneTreeOverNode(const Node* n) { return replaceBy(n, false, true); }
  Node* cloneNodeOverTree(const Node* n) { return replaceBy(n, true, false); }
  Node* cloneTreeOverTree(const Node* n) { return replaceBy(n, true, true); }
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
  void insert(const Node* nodeToInsert, bool before, bool isTree);
  Node* replaceBy(const Node* n, bool oldIsTree, bool newIsTree);
  void detach(bool isTree);
  void remove(bool isTree);
  uint16_t m_identifier;
};

void CloneNodeBeforeNode(EditionReference* target, const Node* nodeToInsert);
void CloneTreeBeforeNode(EditionReference* target, const Node* treeToInsert);

void MoveNodeBeforeNode(EditionReference* target, Node* nodeToInsert);
void MoveTreeBeforeNode(EditionReference* target, Node* treeToInsert);

inline void ReplaceTreeByNode(EditionReference* u, Node* n) {
  *u = u->replaceTreeByNode(n);
}

inline void ReplaceTreeByTree(EditionReference* u, Node* n) {
  *u = u->replaceTreeByTree(n);
}

inline void ReplaceNodeByNode(EditionReference* u, Node* n) {
  *u = u->replaceNodeByNode(n);
}

inline void ReplaceNodeByTree(EditionReference* u, Node* n) {
  *u = u->replaceNodeByTree(n);
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
