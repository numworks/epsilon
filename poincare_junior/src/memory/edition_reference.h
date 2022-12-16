#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "node.h"
#include "edition_pool.h"

namespace Poincare {

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
  inline bool operator==(const EditionReference & t) const { return m_identifier == t.identifier() || (!isUninitialized() && !t.isUninitialized() && node() == t.node()); }
  inline bool operator!=(const EditionReference & t) const { return m_identifier != t.identifier() && (isUninitialized() || t.isUninitialized() || node() != t.node()); }

  bool isUninitialized() const { return m_identifier == EditionPool::ReferenceTable::NoNodeIdentifier; }
  Node node() const;
  TypeBlock * block() { return node().block(); }
  BlockType type() const { return node().type(); }

  uint16_t identifier() const { return m_identifier; }

  /* Hierarchy */
  EditionReference nextNode() { return EditionReference(node().nextNode()); }
  EditionReference nextTree() { return EditionReference(node().nextTree()); }
  EditionReference previousNode() { return EditionReference(node().previousNode()); }
  EditionReference previousTree() { return EditionReference(node().previousTree()); }
  bool hasChild(EditionReference t) const { return node().hasChild(t.node()); }
  bool hasSibling(EditionReference t) const { return node().hasSibling(t.node()); }
  bool hasAncestor(EditionReference t, bool includeSelf) const { return node().hasAncestor(t.node(), includeSelf); }
  int numberOfChildren() const { return node().numberOfChildren(); }
  int indexOfChild(EditionReference t) const { return node().indexOfChild(t.node()); }
  EditionReference parent() const { return node().parent(); }
  EditionReference childAtIndex(int i) const { return EditionReference(node().childAtIndex(i)); }
  int numberOfDescendants(bool includeSelf) const { return node().numberOfDescendants(includeSelf); }

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

  // Edition operations on EditionReference
  void insertNodeAfterNode(EditionReference nodeToInsert) { insertNodeAfterNode(nodeToInsert.node()); }
  void insertTreeAfterNode(EditionReference treeToInsert) { insertTreeAfterNode(treeToInsert.node()); }
  void insertNodeBeforeNode(EditionReference nodeToInsert) { insertNodeBeforeNode(nodeToInsert.node()); }
  void insertTreeBeforeNode(EditionReference treeToInsert) { insertTreeBeforeNode(treeToInsert.node()); }
  void replaceNodeByNode(EditionReference t) { replaceNodeByNode(t.node()); }
  void replaceNodeByTree(EditionReference t) { replaceNodeByTree(t.node()); }
  void replaceTreeByNode(EditionReference t) { replaceTreeByNode(t.node()); }
  void replaceTreeByTree(EditionReference t) { replaceTreeByTree(t.node()); }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

private:
  void insert(Node nodeToInsert, bool before, bool isTree);
  void replaceBy(Node n, bool oldIsTree, bool newIsTree);
  void remove(bool isTree);
  uint16_t m_identifier;
};

}

#endif
