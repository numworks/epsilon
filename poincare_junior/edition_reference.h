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
  void replaceNodeBy(EditionReference t) { replaceBy(t, false); }
  void replaceTreeBy(EditionReference t) { replaceBy(t, true); }
  void insertNodeAfter(EditionReference t) { insert(t, false, false); }
  void insertTreeAfter(EditionReference t) { insert(t, false, true); }
  void insertNodeBefore(EditionReference t) { insert(t, true, false); }
  void insertTreeBefore(EditionReference t) { insert(t, true, true); }
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }

private:
  void insert(EditionReference t, bool before, bool insertTree);
  void replaceBy(EditionReference t, bool replaceTrees);
  void remove(bool removeTree);
  uint16_t m_identifier;
};

class ReferenceIterator {
public:
  ReferenceIterator(const EditionReference reference) : m_reference(reference) {}

  class Direct {
  public:
    Direct(const EditionReference reference) : m_reference(reference) {}
    class Iterator {
    public:
      Iterator(EditionReference reference) : m_reference(reference) {}
      EditionReference operator*() { return m_reference; }
      bool operator!=(const Iterator& it) const { return (m_reference != it.m_reference); }
    protected:
      EditionReference m_reference;
    };
  protected:
    const EditionReference m_reference;
  };

  class ForwardDirect final : public Direct {
    using Direct::Direct;
  public:
    class Iterator : public Direct::Iterator {
    public:
      using Direct::Iterator::Iterator;
      Iterator & operator++() {
        m_reference = EditionReference(m_reference.node().nextTree());
        return *this;
      }
    };
    Iterator begin() const { return Iterator(EditionReference(m_reference.node().nextNode())); }
    Iterator end() const { return Iterator(EditionReference(m_reference.node().nextTree())); }
  };

  class BackwardsDirect final : public Direct {
  public:
    using Direct::Direct;
    class Iterator : public Direct::Iterator {
    public:
      using Direct::Iterator::Iterator;
      Iterator & operator++() {
        m_reference = EditionReference(m_reference.node().previousTree());
        return *this;
      }
    };
    Iterator begin() const { return Iterator(EditionReference(m_reference.node().nextTree().previousNode())); }
    Iterator end() const { return Iterator(EditionReference(Node())); }
  };

  ForwardDirect directChildren() { return ForwardDirect(m_reference); }
  BackwardsDirect backwardsDirectChildren() { return BackwardsDirect(m_reference); }

private:
  const EditionReference m_reference;
};

}

#endif
