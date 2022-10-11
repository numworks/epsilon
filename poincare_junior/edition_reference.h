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

class Iterator {
  /* You can iterate forward and backward but can only modify the children
   * downstream (the children after the current child if you're going forwards
   * and the children before the current child if you're going backwards). */
public:
  Iterator(const Node node) : m_node(node) {}

  struct IndexedNode {
    Node m_node;
    int m_index;
  };

  class ScanChildren {
  public:
    ScanChildren(const Node node) : m_node(node) {}
    class Iterator {
    public:
      Iterator(EditionReference reference = EditionReference()) : m_reference(reference), m_index(0) {}
      IndexedNode operator*() { return {.m_node = getNode(), .m_index = m_index}; }
      bool operator!=(const Iterator& it) const { return (m_reference.node() != it.m_reference.node()); }
      Iterator & operator++() {
        setNode(incrNode(getNode()));
        m_index++;
        return *this;
      }
    protected:
      /* Hack: we keep a reference to a block right before (or after) the
       * currenNode to handle cases where the current node is replaced by
       * another one. The assertion that the previous children aren't modified
       * ensure the validity of this hack. */
      virtual Node getNode() { return Node(m_reference.node().block() + delta()); }
      virtual void setNode(Node node) { m_reference = EditionReference(Node(node.block() - delta())); }
      EditionReference m_reference;
      int m_index;
    private:
      virtual Node incrNode(Node node) = 0;
      virtual int delta() = 0;
    };
  protected:
    const Node m_node;
  };

  class ForwardChildren final : public ScanChildren {
    using ScanChildren::ScanChildren;
  public:
    class Iterator final : public ScanChildren::Iterator {
    public:
      Iterator(const Node node) : ScanChildren::Iterator() { setNode(node); }
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
      int delta() override { return 1; }
    };
    Iterator begin() const { return Iterator(m_node.nextNode()); }
    Iterator end() const { return Iterator(m_node.nextTree()); }
  };

  class BackwardsChildren final : public ScanChildren {
  public:
    using ScanChildren::ScanChildren;
    class Iterator final : public ScanChildren::Iterator {
    public:
      Iterator(const Node node) : ScanChildren::Iterator(EditionReference(node)) {}
    private:
      /* We can't keep a reference outside the scanned tree so we create
       * an edge case for the right most child: it's referenced by the parent
       * node. */
      Node getNode() override { return m_index == 0 ? m_reference.node().nextTree().previousNode() : ScanChildren::Iterator::getNode(); }
      void setNode(Node node) override {
        if (node.isUninitialized()) {
          m_reference = EditionReference(node);
        } else {
          ScanChildren::Iterator::setNode(node);
        }
      }
      Node incrNode(Node node) override { return node.previousTree(); }
      int delta() override { return -1; }
    };
    Iterator begin() const { return Iterator(m_node); }
    Iterator end() const { return Iterator(Node()); }
  };

  ForwardChildren forwardChildren() { return ForwardChildren(m_node); }
  BackwardsChildren backwardsChildren() { return BackwardsChildren(m_node); }

private:
  const Node m_node;
};

}

#endif
