#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

#include "edition_reference.h"
#include <utils/enums.h>

namespace Poincare {

/*
 * Four types of iterators depending on:
 * - the scanning direction: forward or backward
 * - the editability of the children
 * You can use the editable scan only for trees located in the editable pool.
 * When doing so you can only edit the children downstream (the children after
 * the current child if you're going forwards and the children before the
 * current child if you're going backwards). */

class Iterator {
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

  template<typename T, int N>
  class ConstIterator {
  public:
    std::pair<T, int> operator*() { return std::pair<T, int>(convert(), m_index); }
    bool operator!=(ConstIterator& it) { return (m_index != it.m_index); }
    ConstIterator & operator++() {
      for (Node & node : m_nodes) {
        node = incrNode(node);
      }
      m_index++;
      return *this;
    }
  protected:
    const Node m_nodes[N];
  private:
    T convert();
    virtual Node incrNode() = 0;
    int m_index;
  };

  template<typename T, int N>
  class EditableIterator {
  public:
    std::pair<T, int> operator*() { return std::pair<T, int>(convert(), m_index); }
    bool operator!=(EditableIterator& it) {
      for (size_t i = 0; i < N; i++) {
        if (getNode(i) == it.getNode(i)) {
          return false;
        }
      }
      return true;
    }
    EditableIterator & operator++() {
      for (size_t i = 0; i < N; i++) {
        setNode(i, incrNode(getNode(i)));
      }
      m_index++;
      return *this;
    }
  protected:
    /* Hack: we keep a reference to a block right before (or after) the
     * currenNode to handle cases where the current node is replaced by
     * another one. The assertion that the previous children aren't modified
     * ensure the validity of this hack. */
    virtual Node getNode(int index) { return Node(m_references[index].node().block() + offset()); }
    virtual void setNode(int index Node node) { m_references[index] = EditionReference(Node(node.block() - offset())); }
    T convert();
    virtual Node incrNode(Node node) = 0;
    virtual int offset() = 0;
    EditionReference m_references[N];
    int m_index;
  };
};

class NodeIterator : Iterator {
public:
  template<>
  class ConstIterator<const Node, 1> {
  public:
    ConstIterator(const Node node, int index) : m_nodes{node}, m_index(index) {}
  private:
    const Node convert() { return m_nodes[0]; }
  };

  template <>
  class ChildrenScanner<ScanDirection::Forward, Editable::False, const Node> {
  public:
    ChildrenScanner(const Node node) : m_node(node) {}

    class Iterator : public ConstIterator<const Node, 1> {
    public:
      using ConstIterator<const Node, 1>::ConstIterator<const Node, 1>;
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
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
      Node incrNode(Node node) override { return node.previousTree(); }
    };

    Iterator begin() const { return Iterator(m_node.nextTree().previousNode(), 0); }
    Iterator end() const { return Iterator(Node(), m_node.numberOfChildren()); }

  private:
    const Node m_node;
  };

  template<>
  class EditableIterator<EditionReference, 1> {
  public:
    EditableIterator(EditionReference reference, int index) : m_references{reference}, m_index(index) {}
  private:
    EditionReference convert() { return m_references[1]; }
  };

  template <>
  class ChildrenScanner<ScanDirection::Forward, Editable::True, EditionReference> {
  public:
    ChildrenScanner(EditionReference reference) : m_reference(reference) {}

    class Iterator : public EditableIterator<EditionReference, 1> {
    public:
      Iterator(Node node, int index = 0) : EditableIterator<EditionReference, 1>() { setNode(node); }
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

    class Iterator : public EditableIterator<EditionReference, 1> {
    public:
      using EditableIterator<EditionReference, 1>::EditableIterator<EditionReference, 1>;
    private:
      Node getNode() override {
        if (m_index < 0) {
          // Special case: end iterator
          return Node();
        } else if (m_index == 0) {
          /* We can't keep a reference outside the scanned tree so we create
           * an edge case for the right most child: it's referenced by the parent
           * node. */
          return m_reference.node().nextTree().previousNode();‡
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
  template<>
  class ConstIterator<std::pair<const Node, const Node>, 2> {
  public:
    ConstIterator(const Node node0, const Node node1 int index) : m_nodes{node0, node1}, m_index(index) {}
  private:
    const Node convert() { return std::make_pair<const Node, const Node>(m_nodes[0], m_nodes[1]); }
  };

  template <>
  class ChildrenScanner<ScanDirection::Forward, Editable::False, std::pair<const Node, const Node>> {
  public:
    ChildrenScanner(std::pair<const Node, const Node> nodes) : m_nodes(nodes) {}

    class Iterator : public ConstIterator<std::pair<const Node, const Node>, 2> {
    public:
      using ConstIterator<std::pair<const Node, const Node>, 2>::ConstIterator<std::pair<const Node, const Node>, 2>;
    private:
      Node incrNode(Node node) override { return m_node.nextTree(); }
    };

    Iterator begin() const { return Iterator(m_nodes.get<0>().nextNode(), m_nodes.get<1>().nextNode(), 0); }
    Iterator end() const { return Iterator(Node(), Node(), std::min(m_nodes[0].numberOfChildren(), m_nodes[1].numberOfChildren())); }

  private:
    std::pair<const Node, const Node> m_node;
  };

  template <>
  class ChildrenScanner<ScanDirection::Backward, Editable::False, std::pair<const Node, const Node>> {
  public:
    ChildrenScanner(std::pair<const Node, const Node> nodes) : m_nodes(nodes) {}

    class Iterator : public ConstIterator<std::pair<const Node, const Node>, 2> {
    public:
      using ConstIterator<std::pair<const Node, const Node>, 2>::ConstIterator<std::pair<const Node, const Node>, 2>;
    private:
      Node incrNode(Node node) override { return m_node.previousTree(); }
    };

    Iterator begin() const { return Iterator(m_nodes.get<0>().nextTree().previousNode(), m_nodes.get<1>().nextTree().previousNode(), 0); }
    Iterator end() const { return Iterator(Node(), Node(), std::min(m_nodes[0].numberOfChildren(), m_nodes[1].numberOfChildren())); }

  private:
    std::pair<const Node, const Node> m_nodes;
  };

  template<>
  class EditableIterator<std::pair<EditionReference,EditionReference>, 2> {
  public:
    EditableIterator() : m_references{}, m_index(0) {}
  private:
    std::pair<EditionReference,EditionReference> convert() { return std::make_pair(m_references[0], m_references[1]); }
  };

  template <>
  class ChildrenScanner<ScanDirection::Forward, Editable::True, std::pair<EditionReference,EditionReference>> {
  public:
    ChildrenScanner(std::pair<EditionReference, EditionReference> references) : m_references(references) {}

    class Iterator : public EditableIterator<std::pair<EditionReference,EditionReference>, 2> {
    public:
      Iterator(EditionReference reference0, EditionReference reference1) {
        setNode(0, reference0.node());
        setNode(1, reference1.node());
      }
    private:
      Node incrNode(Node node) override { return node.nextTree(); }
      int offset() override { return static_cast<int>(ScanDirection::Forward); }
    };

    Iterator begin() const { return Iterator(m_reference.node().nextNode()); }
    Iterator end() const { return Iterator(m_reference.node().nextTree()); }

    Iterator begin() const { return Iterator(m_references.get<0>().nextNode(), m_references.get<1>().nextNode()); }
    Iterator end() const { return Iterator(m_references.get<0>().nextTree(), m_references.get<1>().nextTree()); }
  private:
    std::pair<EditionReference, EditionReference> m_references;
  };

  template <ScanDirection direction, Editable isEditable, typename T>
  static ChildrenScanner<direction, isEditable, std::pair<T,T>> Children(T node0, T node1) { return ChildrenScanner<direction, isEditable, std::pair<T,T>>(std::make_pair(node0, node1)); }
};

}

#endif
