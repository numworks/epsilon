#ifndef POINCARE_NODE_ITERATOR_H
#define POINCARE_NODE_ITERATOR_H

#include "edition_reference.h"

namespace Poincare {

class NodeIterator {

  /* Four types of iterators depending on:
   * - the scanning direction: forward or backward
   * - the editability of the children
   * You can use the editable scan only for trees located in the editable pool.
   * When doing so you can only edit the children downstream (the children after
   * the current child if you're going forwards and the children before the
   * current child if you're going backwards). */

public:
  NodeIterator(const Node node) : m_node(node) {}

  struct IndexedNode {
    Node m_node;
    int m_index;
  };

  class ScanChildren {
  public:
    ScanChildren(const Node node) : m_node(node) {}
    class Iterator {
    public:
      Iterator(int index = 0) : m_index(index) {}
      const IndexedNode operator*() { return {.m_node = getNode(), .m_index = m_index}; }
      bool operator!=(const Iterator& it) const { return (getConstNode() != it.getConstNode()); }
      Iterator & operator++() {
        setNode(incrNode(getNode()));
        m_index++;
        return *this;
      }
    protected:
      virtual Node getNode() = 0;
      const Node getConstNode() const { return const_cast<Iterator *>(this)->getNode(); }
      virtual void setNode(Node node) = 0;
      int m_index;
    private:
      virtual Node incrNode(Node node) = 0;
    };
  protected:
    const Node m_node;
  };

  class ScanConstChildren : public ScanChildren {
  public:
    using ScanChildren::ScanChildren;
    class Iterator : public ScanChildren::Iterator {
    public:
      Iterator(const Node node) : ScanChildren::Iterator(), m_node(node) {}
    private:
      Node getNode() override { return m_node; }
      void setNode(Node node) override { m_node = node; }
      Node m_node;
    };
  };

  class ForwardConstChildren final : public ScanConstChildren {
 public:
    using ScanConstChildren::ScanConstChildren;
    class Iterator : public ScanConstChildren::Iterator {
    public:
      using ScanConstChildren::Iterator::Iterator;
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
    };
    Iterator begin() const { return Iterator(m_node.nextNode()); }
    Iterator end() const { return Iterator(m_node.nextTree()); }
  };

  class BackwardsConstChildren final : public ScanConstChildren {
 public:
    using ScanConstChildren::ScanConstChildren;
    class Iterator : public ScanConstChildren::Iterator {
    public:
      using ScanConstChildren::Iterator::Iterator;
    private:
      Node incrNode(Node node) override { return node.previousTree(); }
    };
    Iterator begin() const { return Iterator(m_node.nextTree().previousNode()); }
    Iterator end() const { return Iterator(Node()); }
  };

  class ScanEditableChildren : public ScanChildren {
  public:
    using ScanChildren::ScanChildren;
    class Iterator : public ScanChildren::Iterator {
    public:
      Iterator(EditionReference reference = EditionReference(), int index = 0) : ScanChildren::Iterator(index), m_reference(reference) {}
      IndexedNode operator*() { return {.m_node = getNode(), .m_index = m_index}; }
    protected:
      /* Hack: we keep a reference to a block right before (or after) the
       * currenNode to handle cases where the current node is replaced by
       * another one. The assertion that the previous children aren't modified
       * ensure the validity of this hack. */
      Node getNode() override { return Node(m_reference.node().block() + delta()); }
      void setNode(Node node) override { m_reference = EditionReference(Node(node.block() - delta())); }
      EditionReference m_reference;
    private:
      virtual int delta() = 0;
    };
  };

  class ForwardEditableChildren final : public ScanEditableChildren {
    using ScanEditableChildren::ScanEditableChildren;
  public:
    class Iterator final : public ScanEditableChildren::Iterator {
    public:
      Iterator(const Node node) : ScanEditableChildren::Iterator() { setNode(node); }
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
      int delta() override { return 1; }
    };
    Iterator begin() const { return Iterator(m_node.nextNode()); }
    Iterator end() const { return Iterator(m_node.nextTree()); }
  };

  /* This code is UGLY, please do something. */
  class BackwardsEditableChildren final : public ScanEditableChildren {
  public:
    using ScanEditableChildren::ScanEditableChildren;
    class Iterator final : public ScanEditableChildren::Iterator {
    public:
      Iterator(const Node node, int index) : ScanEditableChildren::Iterator(EditionReference(node), index) {}
    private:
      Node getNode() override {
        if (m_index < 0) {
          // Special case: end iterator
          return Node();
        } else if (m_index == 0) {
          /* We can't keep a reference outside the scanned tree so we create
           * an edge case for the right most child: it's referenced by the parent
           * node. */
          return m_reference.node().nextTree().previousNode();
        }
        return ScanEditableChildren::Iterator::getNode();
      }
      void setNode(Node node) override {
        if (node.isUninitialized()) {
          // Special case: end iterator
          m_index = -2;
          return;
        }
        ScanEditableChildren::Iterator::setNode(node);
      }
      Node incrNode(Node node) override { return node.previousTree(); }
      int delta() override { return -1; }
    };
    Iterator begin() const { return Iterator(m_node, 0); }
    Iterator end() const { return Iterator(Node(), -1); }
  };

  ForwardConstChildren forwardConstChildren() { return ForwardConstChildren(m_node); }
  BackwardsConstChildren backwardsConstChildren() { return BackwardsConstChildren(m_node); }

  ForwardEditableChildren forwardEditableChildren() { return ForwardEditableChildren(m_node); }
  BackwardsEditableChildren backwardsEditableChildren() { return BackwardsEditableChildren(m_node); }

private:
  const Node m_node;
};

}

#endif

