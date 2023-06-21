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

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  /* Comparison */
  inline bool operator==(const EditionReference& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() &&
            static_cast<Node*>(*this) == static_cast<Node*>(t));
  }
  inline bool operator!=(const EditionReference& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() ||
            static_cast<Node*>(*this) != static_cast<Node*>(t));
  }

  EditionReference clone() {
    return EditionPool::sharedEditionPool()->clone(*this);
  }

  operator const Node*() const;
  bool isUninitialized() const {
    return static_cast<Node*>(*this).isUninitialized();
  }
  TypeBlock* block() { return static_cast<Node*>(*this).block(); }
  BlockType type() const { return static_cast<Node*>(*this).type(); }

  uint16_t identifier() const { return m_identifier; }

  /* Hierarchy */
  Node* nextNode() { return static_cast<Node*>(*this).nextNode(); }
  Node* nextTree() { return static_cast<Node*>(*this).nextTree(); }
  Node* previousNode() { return static_cast<Node*>(*this).previousNode(); }
  Node* previousTree() { return static_cast<Node*>(*this).previousTree(); }
  bool hasChild(EditionReference t) const {
    return static_cast<Node*>(*this).hasChild(t);
  }
  bool hasSibling(EditionReference t) const {
    return static_cast<Node*>(*this).hasSibling(t);
  }
  bool hasAncestor(EditionReference t, bool includeSelf) const {
    return static_cast<Node*>(*this).hasAncestor(t, includeSelf);
  }
  int numberOfChildren() const {
    return static_cast<Node*>(*this).numberOfChildren();
  }
  int indexOfChild(EditionReference t) const {
    return static_cast<Node*>(*this).indexOfChild(t);
  }
  Node* parent() const { return static_cast<Node*>(*this).parent(); }
  Node* childAtIndex(int i) const {
    return static_cast<Node*>(*this).childAtIndex(i);
  }
  int numberOfDescendants(bool includeSelf) const {
    return static_cast<Node*>(*this).numberOfDescendants(includeSelf);
  }

  /* Edition operations on Node* */
  void insertNodeAfterNode(Node* nodeToInsert) {
    insert(nodeToInsert, false, false);
  }
  void insertTreeAfterNode(Node* nodeToInsert) {
    insert(nodeToInsert, false, true);
  }
  void insertNodeBeforeNode(Node* nodeToInsert) {
    insert(nodeToInsert, true, false);
  }
  void insertTreeBeforeNode(Node* nodeToInsert) {
    insert(nodeToInsert, true, true);
  }
  Node* replaceNodeByNode(Node* n) { return replaceBy(n, false, false); }
  Node* replaceNodeByTree(Node* n) { return replaceBy(n, false, true); }
  Node* replaceTreeByNode(Node* n) { return replaceBy(n, true, false); }
  Node* replaceTreeByTree(Node* n) { return replaceBy(n, true, true); }
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }
  void detachNode() { detach(false); }
  void detachTree() { detach(true); }

  // Edition operations on EditionReference
  void insertNodeAfterNode(EditionReference nodeToInsert) {
    insertNodeAfterNode(static_cast<Node*>(nodeToInsert));
  }
  void insertTreeAfterNode(EditionReference treeToInsert) {
    insertTreeAfterNode(static_cast<Node*>(treeToInsert));
  }
  void insertNodeBeforeNode(EditionReference nodeToInsert) {
    insertNodeBeforeNode(static_cast<Node*>(nodeToInsert));
  }
  void insertTreeBeforeNode(EditionReference treeToInsert) {
    insertTreeBeforeNode(static_cast<Node*>(treeToInsert));
  }
  Node* replaceNodeByNode(EditionReference t) {
    return replaceNodeByNode(static_cast<Node*>(t));
  }
  Node* replaceNodeByTree(EditionReference t) {
    return replaceNodeByTree(static_cast<Node*>(t));
  }
  Node* replaceTreeByNode(EditionReference t) {
    return replaceTreeByNode(static_cast<Node*>(t));
  }
  Node* replaceTreeByTree(EditionReference t) {
    return replaceTreeByTree(static_cast<Node*>(t));
  }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

  EditionReference matchAndCreate(const Node* pattern,
                                  const Node* structure) const;
  // Return true if reference has been replaced.
  bool matchAndReplace(const Node* pattern, const Node* structure);

 private:
  void insert(Node* nodeToInsert, bool before, bool isTree);
  Node* replaceBy(Node* n, bool oldIsTree, bool newIsTree);
  void detach(bool isTree);
  void remove(bool isTree);
  uint16_t m_identifier;
};

void InsertNodeBeforeNode(EditionReference* target, Node* nodeToInsert);
void InsertTreeBeforeNode(EditionReference* target, Node* treeToInsert);

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

void SwapTrees(EditionReference* u, EditionReference* v);

}  // namespace PoincareJ

#endif
