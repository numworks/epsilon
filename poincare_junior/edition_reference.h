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
  void replaceBy(EditionReference t);
  void insertAfter(EditionReference t) { insert(t, false); }
  void insertBefore(EditionReference t) { insert(t, true); }
  void remove();

private:
  void insert(EditionReference t, bool before);
  uint16_t m_identifier;
};

}

#endif
