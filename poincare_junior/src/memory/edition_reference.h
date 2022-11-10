#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "node.h"

namespace Poincare {

class EditionReference {

public:
  EditionReference(Node node = Node());

  /* Comparison */
  inline bool operator==(const EditionReference & t) const { return m_identifier == t.identifier(); }
  inline bool operator!=(const EditionReference & t) const { return m_identifier != t.identifier(); }

  Node node() const;
  TypeBlock * block() { return node().block(); }
  EditionReference clone() const;

  uint16_t identifier() const { return m_identifier; }

  /* Hierarchy */
  bool hasChild(EditionReference t) const { return node().hasChild(t.node()); }
  bool hasSibling(EditionReference t) const { return node().hasSibling(t.node()); }
  bool hasAncestor(EditionReference t, bool includeSelf) const { return node().hasAncestor(t.node(), includeSelf); }
  int numberOfChildren() const { return node().numberOfChildren(); }
  int indexOfChild(EditionReference t) const { return node().indexOfChild(t.node()); }
  EditionReference parent() const { return node().parent(); }
  EditionReference childAtIndex(int i) const { return EditionReference(node().childAtIndex(i)); }
  int numberOfDescendants(bool includeSelf) const { return node().numberOfDescendants(includeSelf); }

  /* Edition operations on Node */
  void insertNodeAfterNode(Node n) { insert(n, false, false); }
  void insertTreeAfterNode(Node n) { insert(n, false, true); }
  void insertNodeBeforeNode(Node n) { insert(n, true, false); }
  void insertTreeBeforeNode(Node n) { insert(n, true, true); }
  void replaceNodeByNode(Node n) { replaceBy(n, false, false); }
  void replaceNodeByTree(Node n) { replaceBy(n, false, true); }
  void replaceTreeByNode(Node n) { replaceBy(n, true, false); }
  void replaceTreeByTree(Node n) { replaceBy(n, true, true); }
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }

  // Edition operations on EditionReference
  void insertNodeAfterNode(EditionReference t) { insertNodeAfterNode(t.node()); }
  void insertTreeAfterNode(EditionReference t) { insertTreeAfterNode(t.node()); }
  void insertNodeBeforeNode(EditionReference t) { insertNodeBeforeNode(t.node()); }
  void insertTreeBeforeNode(EditionReference t) { insertTreeBeforeNode(t.node()); }
  void replaceNodeByNode(EditionReference t) { replaceNodeByNode(t.node()); }
  void replaceNodeByTree(EditionReference t) { replaceNodeByTree(t.node()); }
  void replaceTreeByNode(EditionReference t) { replaceTreeByNode(t.node()); }
  void replaceTreeByTree(EditionReference t) { replaceTreeByTree(t.node()); }

private:
  void insert(Node n, bool before, bool isTree);
  void replaceBy(Node n, bool oldIsTree, bool newIsTree);
  void remove(bool isTree);
  uint16_t m_identifier;
};

}

#endif
