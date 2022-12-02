#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "node.h"
#include "edition_pool.h"

namespace PoincareJ {

class EditionReference {

public:
  EditionReference() : m_identifier(EditionPool::ReferenceTable::NoNodeIdentifier) {}
  EditionReference(Node node);

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  template <BlockType blockType, typename... Types>
  static EditionReference Push(Types... args);
  static EditionReference Clone(const Node node);

  /* Comparison */
  inline bool operator==(const EditionReference & t) const { return m_identifier == t.identifier() || (!isUninitialized() && !t.isUninitialized() && static_cast<Node>(*this) == static_cast<Node>(t)); }
  inline bool operator!=(const EditionReference & t) const { return m_identifier != t.identifier() && (isUninitialized() || t.isUninitialized() || static_cast<Node>(*this) != static_cast<Node>(t)); }

  bool isUninitialized() const { return m_identifier == EditionPool::ReferenceTable::NoNodeIdentifier; }
  operator const Node() const;
  TypeBlock * block() { return static_cast<Node>(*this).block(); }
  BlockType type() const { return static_cast<Node>(*this).type(); }

  uint16_t identifier() const { return m_identifier; }

  /* Hierarchy */
  EditionReference nextNode() { return EditionReference(static_cast<Node>(*this).nextNode()); }
  EditionReference nextTree() { return EditionReference(static_cast<Node>(*this).nextTree()); }
  EditionReference previousNode() { return EditionReference(static_cast<Node>(*this).previousNode()); }
  EditionReference previousTree() { return EditionReference(static_cast<Node>(*this).previousTree()); }
  bool hasChild(EditionReference t) const { return static_cast<Node>(*this).hasChild(t); }
  bool hasSibling(EditionReference t) const { return static_cast<Node>(*this).hasSibling(t); }
  bool hasAncestor(EditionReference t, bool includeSelf) const { return static_cast<Node>(*this).hasAncestor(t, includeSelf); }
  int numberOfChildren() const { return static_cast<Node>(*this).numberOfChildren(); }
  int indexOfChild(EditionReference t) const { return static_cast<Node>(*this).indexOfChild(t); }
  EditionReference parent() const { return static_cast<Node>(*this).parent(); }
  EditionReference childAtIndex(int i) const { return EditionReference(static_cast<Node>(*this).childAtIndex(i)); }
  int numberOfDescendants(bool includeSelf) const { return static_cast<Node>(*this).numberOfDescendants(includeSelf); }

  /* Edition operations on Node */
  void insertNodeAfterNode(Node nodeToInsert) { insert(nodeToInsert, false, false); }
  void insertTreeAfterNode(Node nodeToInsert) { insert(nodeToInsert, false, true); }
  void insertNodeBeforeNode(Node nodeToInsert) { insert(nodeToInsert, true, false); }
  void insertTreeBeforeNode(Node nodeToInsert) { insert(nodeToInsert, true, true); }
  void replaceNodeByNode(Node n) { replaceBy(n, false, false); }
  void replaceNodeByTree(Node n) { replaceBy(n, false, true); }
  void replaceTreeByNode(Node n) { replaceBy(n, true, false); }
  void replaceTreeByTree(Node n) { replaceBy(n, true, true); }
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }
  void detachNode() { detach(false); }
  void detachTree() { detach(true); }

  // Edition operations on EditionReference
  void insertNodeAfterNode(EditionReference nodeToInsert) { insertNodeAfterNode(static_cast<Node>(nodeToInsert)); }
  void insertTreeAfterNode(EditionReference treeToInsert) { insertTreeAfterNode(static_cast<Node>(treeToInsert)); }
  void insertNodeBeforeNode(EditionReference nodeToInsert) { insertNodeBeforeNode(static_cast<Node>(nodeToInsert)); }
  void insertTreeBeforeNode(EditionReference treeToInsert) { insertTreeBeforeNode(static_cast<Node>(treeToInsert)); }
  void replaceNodeByNode(EditionReference t) { replaceNodeByNode(static_cast<Node>(t)); }
  void replaceNodeByTree(EditionReference t) { replaceNodeByTree(static_cast<Node>(t)); }
  void replaceTreeByNode(EditionReference t) { replaceTreeByNode(static_cast<Node>(t)); }
  void replaceTreeByTree(EditionReference t) { replaceTreeByTree(static_cast<Node>(t)); }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

private:
  void insert(Node nodeToInsert, bool before, bool isTree);
  void replaceBy(Node n, bool oldIsTree, bool newIsTree);
  void detach(bool isTree);
  void remove(bool isTree);
  uint16_t m_identifier;
};

}

#endif
