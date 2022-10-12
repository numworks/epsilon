#ifndef POINCARE_EDITION_REFERENCE_H
#define POINCARE_EDITION_REFERENCE_H

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
  void replaceNodeBy(EditionReference t) { replaceNodeBy(t.node()); }
  void replaceNodeBy(Node n) { replaceBy(n, false); }
  void replaceTreeBy(EditionReference t) { replaceTreeBy(t.node()); }
  void replaceTreeBy(Node n) { replaceBy(n, true); }
  void insertNodeAfter(EditionReference t) { insertNodeAfter(t.node()); }
  void insertNodeAfter(Node n) { insert(n, false, false); }
  void insertTreeAfter(EditionReference t) { insertTreeAfter(t.node()); }
  void insertTreeAfter(Node n) { insert(n, false, true); }
  void insertNodeBefore(EditionReference t) { insertNodeBefore(t.node()); }
  void insertNodeBefore(Node n) { insert(n, true, false); }
  void insertTreeBefore(EditionReference t) { insertTreeBefore(t.node()); }
  void insertTreeBefore(Node n) { insert(n, true, true); }
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }

private:
  void insert(Node n, bool before, bool insertTree);
  void replaceBy(Node n, bool replaceTrees);
  void remove(bool removeTree);
  uint16_t m_identifier;
};

}

#endif
