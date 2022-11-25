#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

#include "edition_reference.h"
#include <utils/enums.h>

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
  template <ScanDirection direction, Editable isEditable, typename T>
  class ChildrenScanner {
  public:
    ChildrenScanner(T node);

    class Iterator {
    public:
      std::pair<T, int> operator*();
      bool operator!=(Iterator& it);
      Iterator & operator++();
    };

    Iterator begin() const;
    Iterator end() const;
  };

  class ConstIterator {
  public:
    ConstIterator(const Node node, int index) : m_node(node), m_index(index) {}
    std::pair<const Node, int> operator*() { return std::pair<const Node, int>(m_node, m_index); }
    bool operator!=(ConstIterator& it) { return (m_index != it.m_index); }
    ConstIterator & operator++() {
      m_node = incrNode();
      m_index++;
      return *this;
    }
  protected:
    Node m_node;
  private:
    virtual Node incrNode() = 0;
    int m_index;
  };

  template <>
  class ChildrenScanner<ScanDirection::Forward, Editable::False, const Node> {
  public:
    ChildrenScanner(const Node node) : m_node(node) {}

    class Iterator : public ConstIterator {
    public:
      using ConstIterator::ConstIterator;
    private:
      Node incrNode() override { return m_node.nextTree(); }
    };

    Iterator begin() const { return Iterator(m_node.nextNode(), 0); }
    Iterator end() const { return Iterator(Node(), m_node.numberOfChildren()); }

  private:
    const Node m_node;
  };

  template <>
  class ChildrenScanner<ScanDirection::Backward, Editable::False, const Node> {
  public:
    ChildrenScanner(const Node node) : m_node(node) {}

    class Iterator : public ConstIterator {
    public:
      using ConstIterator::ConstIterator;
    private:
      Node incrNode() override { return m_node.previousTree(); }
    };

    Iterator begin() const { return Iterator(m_node.nextTree().previousNode(), 0); }
    Iterator end() const { return Iterator(Node(), m_node.numberOfChildren()); }

  private:
    const Node m_node;
  };

  class EditableIterator {
  public:
    EditableIterator(EditionReference reference = EditionReference(), int index = 0) : m_reference(reference), m_index(index) {}
    std::pair<EditionReference, int> operator*() { return std::pair<EditionReference, int>(EditionReference(getNode()), m_index); }
    bool operator!=(EditableIterator& it) { return (getNode() != it.getNode()); }
    EditableIterator & operator++() {
      setNode(incrNode(getNode()));
      m_index++;
      return *this;
    }
  protected:
    /* Hack: we keep a reference to a block right before (or after) the
     * currenNode to handle cases where the current node is replaced by
     * another one. The assertion that the previous children aren't modified
     * ensure the validity of this hack. */
    virtual Node getNode() { return Node(m_reference.node().block() + offset()); }
    virtual void setNode(Node node) { m_reference = EditionReference(Node(node.block() - offset())); }
    virtual Node incrNode(Node node) = 0;
    virtual int offset() = 0;
    EditionReference m_reference;
    int m_index;
  };

  template <>
  class ChildrenScanner<ScanDirection::Forward, Editable::True, EditionReference> {
  public:
    ChildrenScanner(EditionReference reference) : m_reference(reference) {}

    class Iterator : public EditableIterator {
    public:
      Iterator(Node node, int index = 0) : EditableIterator() { setNode(node); }
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
      int offset() override { return static_cast<int>(ScanDirection::Forward); }
    };

    Iterator begin() const { return Iterator(m_reference.node().nextNode()); }
    Iterator end() const { return Iterator(m_reference.node().nextTree()); }

  private:
    EditionReference m_reference;
  };

  /* This code is UGLY, please do something. */
  template <>
  class ChildrenScanner<ScanDirection::Backward, Editable::True, EditionReference> {
  public:
    ChildrenScanner(EditionReference reference) : m_reference(reference) {}

    class Iterator : public EditableIterator {
    public:
      using EditableIterator::EditableIterator;
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
        return EditableIterator::getNode();
      }
      void setNode(Node node) override {
        if (node.isUninitialized()) {
          // Special case: end iterator
          m_index = -2;
          return;
        }
        EditableIterator::setNode(node);
      }
      Node incrNode(Node node) override { return node.previousTree(); }
      int offset() override { return static_cast<int>(ScanDirection::Backward); }
    };

    Iterator begin() const { return Iterator(m_reference, 0); }
    Iterator end() const { return Iterator(EditionReference(), -1); }

  private:
    EditionReference m_reference;
  };

  template <ScanDirection direction, Editable isEditable, typename T>
  static ChildrenScanner<direction, isEditable, T> Children(T node) { return ChildrenScanner<direction, isEditable, T>(node); }

  /* Workaround: don't emit the undefined Children<ScanDirection::?, Editable::False, Node>
   * but fallback to Children<ScanDirection::?, Editable::False, const Node>. */
  template <ScanDirection direction, Editable isEditable>
  static ChildrenScanner<direction, isEditable, const Node> Children(Node node) { return ChildrenScanner<direction, isEditable, const Node>(node); }
};

class TwoNodesIterator {
public:
  class ConstChildrenScanner {
  public:
    ConstChildrenScanner(const Node node0, const Node node1) : m_nodes{node0, node1} {}
    class Iterator {
    public:
      Iterator(const Node node0, const Node node1, int index) : m_nodes{node0, node1}, m_index(index) {}
      std::pair<Node, Node> operator*() { return std::pair<Node, Node>(m_nodes[0], m_nodes[1]); }
      bool operator!=(const Iterator& it) const { return m_index != it.m_index; }
      Iterator & operator++() {
        for (Node & node : m_nodes) {
          node = incrNode(node);
        }
        m_index++;
        return *this;
      }
    private:
      virtual Node incrNode(Node node) = 0;
      Node m_nodes[2];
      int m_index;
    };
  protected:
    Node m_nodes[2];
  };

  class ForwardConstChildrenScanner final : public ConstChildrenScanner {
 public:
    using ConstChildrenScanner::ConstChildrenScanner;
    class Iterator : public ConstChildrenScanner::Iterator {
    public:
      using ConstChildrenScanner::Iterator::Iterator;
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
    };
    Iterator begin() const { return Iterator(m_nodes[0].nextNode(), m_nodes[1].nextNode(), 0); }
    Iterator end() const { return Iterator(Node(), Node(), std::min(m_nodes[0].numberOfChildren(), m_nodes[1].numberOfChildren())); }
  };

  class BackwardConstChildrenScanner final : public ConstChildrenScanner {
  public:
    using ConstChildrenScanner::ConstChildrenScanner;
    class Iterator : public ConstChildrenScanner::Iterator {
    public:
      using ConstChildrenScanner::Iterator::Iterator;
    private:
      Node incrNode(Node node) override { return node.previousTree(); }
    };
    Iterator begin() const { return Iterator(m_nodes[0].nextTree().previousNode(), m_nodes[1].nextTree().previousNode(), 0); }
    Iterator end() const { return Iterator(Node(), Node(), std::min(m_nodes[0].numberOfChildren(), m_nodes[1].numberOfChildren())); }
  };

  class ForwardEditableChildrenScanner final {
  public:
    ForwardEditableChildrenScanner(EditionReference ref0, EditionReference ref1) : m_nodes{ref0.node(), ref1.node()} {}

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

  static ForwardConstChildrenScanner ForwardConstChildren(const Node node0, const Node node1) { return ForwardConstChildrenScanner(node0, node1); }
  static BackwardConstChildrenScanner BackwardConstChildren(const Node node0, const Node node1) { return BackwardConstChildrenScanner(node0, node1); }
  static ForwardEditableChildrenScanner ForwardEditableChildren(EditionReference ref0, EditionReference ref1) { return ForwardEditableChildrenScanner(ref0, ref1); }
private:
  const Node m_secondNode;
};

}

#endif
