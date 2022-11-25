#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

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
  class ConstChildrenScanner {
  public:
    ConstChildrenScanner(const Node node) : m_node(node) {}

    class Iterator {
    public:
      Iterator(const Node node, int index) : m_node(node), m_index(index) {}
      const std::pair<Node, int> operator*() { return std::pair<Node, int>(m_node, m_index); }
      bool operator!=(const Iterator& it) const { return (m_index != it.m_index); }
      Iterator & operator++() {
        m_node = incrNode(m_node);
        m_index++;
        return *this;
      }
    private:
      virtual Node incrNode(Node node) = 0;
      Node m_node;
      int m_index;
    };
  protected:
    const Node m_node;
  };

  class ForwardConstScanner final : public ConstChildrenScanner {
 public:
    using ConstChildrenScanner::ConstChildrenScanner;
    class Iterator : public ConstChildrenScanner::Iterator {
    public:
      using ConstChildrenScanner::Iterator::Iterator;
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
    };
    Iterator begin() const { return Iterator(m_node.nextNode(), 0); }
    Iterator end() const { return Iterator(Node(), m_node.numberOfChildren()); }
  };

  class BackwardConstScanner final : public ConstChildrenScanner {
  public:
    using ConstChildrenScanner::ConstChildrenScanner;
    class Iterator : public ConstChildrenScanner::Iterator {
    public:
      using ConstChildrenScanner::Iterator::Iterator;
    private:
      Node incrNode(Node node) override { return node.previousTree(); }
    };
    Iterator begin() const { return Iterator(m_node.nextTree().previousNode(), 0); }
    Iterator end() const { return Iterator(Node(), m_node.numberOfChildren()); }
  };

  class EditableChildrenScanner {
  public:
    EditableChildrenScanner(EditionReference reference) : m_node(reference.node()) {}
    class Iterator {
    public:
      Iterator(EditionReference reference = EditionReference(), int index = 0) : m_reference(reference), m_index(index) {}
      std::pair<EditionReference, int> operator*() { return std::pair<EditionReference, int>(EditionReference(getNode()), m_index); }
      bool operator!=(const Iterator& it) const { return (const_cast<Iterator *>(this)->getNode() != const_cast<Iterator *>(&it)->getNode()); }
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
    Node m_node;
  };

  class ForwardEditableScanner final : public EditableChildrenScanner {
    using EditableChildrenScanner::EditableChildrenScanner;
  public:
    class Iterator final : public EditableChildrenScanner::Iterator {
    public:
      Iterator(Node node) : EditableChildrenScanner::Iterator() { setNode(node); }
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
      int delta() override { return 1; }
    };
    Iterator begin() { return Iterator(m_node.nextNode()); }
    Iterator end() { return Iterator(m_node.nextTree()); }
  };

  /* This code is UGLY, please do something. */
  class BackwardEditableScanner final : public EditableChildrenScanner {
  public:
    using EditableChildrenScanner::EditableChildrenScanner;
    class Iterator final : public EditableChildrenScanner::Iterator {
    public:
      Iterator(Node node, int index) : EditableChildrenScanner::Iterator(EditionReference(node), index) {}
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
        return EditableChildrenScanner::Iterator::getNode();
      }
      void setNode(Node node) override {
        if (node.isUninitialized()) {
          // Special case: end iterator
          m_index = -2;
          return;
        }
        EditableChildrenScanner::Iterator::setNode(node);
      }
      Node incrNode(Node node) override { return node.previousTree(); }
      int delta() override { return -1; }
    };
    Iterator begin() const { return Iterator(m_node, 0); }
    Iterator end() const { return Iterator(Node(), -1); }
  };

  static ForwardConstScanner ForwardConstChildren(const Node node) { return ForwardConstScanner(node); }
  static BackwardConstScanner BackwardConstChildren(const Node node) { return BackwardConstScanner(node); }

  static ForwardEditableScanner ForwardEditableChildren(EditionReference reference) { return ForwardEditableScanner(reference); }
  static BackwardEditableScanner BackwardEditableChildren(EditionReference reference) { return BackwardEditableScanner(reference); }
};

class TwoNodesIterator : public NodeIterator {
public:
  class ForwardConstScanner final {
  public:
    ForwardConstScanner(const Node node0, const Node node1) : m_nodes{node0, node1} {}
    class Iterator {
    public:
      Iterator(const Node node0, const Node node1, int index) : m_nodes{node0, node1}, m_index(index) {}
      std::pair<Node, Node> operator*() { return std::pair<Node, Node>(m_nodes[0], m_nodes[1]); }
      bool operator!=(const Iterator& it) const { return m_index != it.m_index; }
      Iterator & operator++() {
        for (Node & node : m_nodes) {
          node = node.nextTree();
        }
        m_index++;
        return *this;
      }
    private:
      Node m_nodes[2];
      int m_index;
    };
    Iterator begin() const { return Iterator(m_nodes[0].nextNode(), m_nodes[1].nextNode(), 0); }
    Iterator end() const { return Iterator(Node(), Node(), std::min(m_nodes[0].numberOfChildren(), m_nodes[1].numberOfChildren())); }
  private:
    Node m_nodes[2];
  };


  class ForwardEditableScanner final {
  public:
    ForwardEditableScanner(EditionReference ref0, EditionReference ref1) : m_nodes{ref0.node(), ref1.node()} {}

    class Iterator {
    public:
      Iterator(const Node node0, const Node node1) {
        setNode(0, node0);
        setNode(1, node1);
      }
      const std::pair<EditionReference, EditionReference> operator*() { return std::pair<EditionReference, EditionReference>(EditionReference(getNode(0)), EditionReference(getNode(1))); }
      bool operator!=(Iterator& it) { return (getNode(0) != it.getNode(0) || getNode(1) != it.getNode(1)); } // We stop iterating as soon as we reach the last child of either node
      Iterator & operator++() {
        for (size_t i = 0; i < sizeof(m_references)/sizeof(m_references[0]); i++) {
          setNode(i, getNode(i).nextTree());
        }
        return *this;
      }
    private:
      /* Hack: we keep a reference to a block right before (or after) the
       * currenNode to handle cases where the current node is replaced by
       * another one. The assertion that the previous children aren't modified
       * ensure the validity of this hack. */
      constexpr static int delta = 1;
      Node getNode(int index) { return Node(m_references[index].node().block() + delta); }
      void setNode(int index, Node node) { m_references[index] = EditionReference(Node(node.block() - delta)); }
      EditionReference m_references[2];
    };

    Iterator begin() const { return Iterator(m_nodes[0].nextNode(), m_nodes[1].nextNode()); }
    Iterator end() const { return Iterator(m_nodes[0].nextTree(), m_nodes[1].nextTree()); }
  private:
    Node m_nodes[2];
  };

  static ForwardConstScanner ForwardConstChildren(const Node node0, const Node node1) { return ForwardConstScanner(node0, node1); }
  static ForwardEditableScanner ForwardEditableChildren(EditionReference ref0, EditionReference ref1) { return ForwardEditableScanner(ref0, ref1); }
private:
  const Node m_secondNode;
};

}

#endif
