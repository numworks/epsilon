#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

#include <array>
#include "edition_reference.h"
#include <utils/array.h>

namespace Poincare {

class MultipleNodesIterator {

  /* Generic templated scanners and iterators, please choose:
   * - the scanning direction: forward or backward
   * - the editability of nodes: True or False
   * - the number of nodes we're iterating through
   *
   * The typename T is always:
   * T = EditionReference if Editable::True
   * T = const Node if Editable::False
   *
   * For instance: ChildrenScanner<ScanDirection::Forward, Editable::True, EditionReference, 2>
   * is a scanner concomittantly iterating through 2 nodes' children.
   *
   * Please note:
   * You can use the editable scan only for trees located in the editable pool.
   * When doing so you can only edit the children downstream (the children after
   * the current child if you're going forwards and the children before the
   * current child if you're going backwards).
   */
protected:
  /* Iterator */

  template <typename DirectionPolicy, typename EditablePolicy, size_t N>
  class Iterator : private DirectionPolicy, private EditablePolicy {
  public:
    typedef typename EditablePolicy::NodeType NodeType;
    typedef std::array<NodeType ,N> ArrayType;

    Iterator(ArrayType array, int index) : m_array(convertToArrayType(convertFromArrayType(array), offset())), m_index(index) {}
    std::pair<ArrayType, int> operator*() { return std::pair(convertToArrayType(convertFromArrayType(m_array, offset())), m_index); }
    bool operator!=(Iterator<DirectionPolicy, EditablePolicy, N> & it) { return equality(m_array, m_index, it.m_array, it.m_index); }
    Iterator<DirectionPolicy, EditablePolicy, N> & operator++() {
      m_array = convertToArrayType(incrementeArray(convertFromArrayType(m_array, offset())), offset());
      m_index++;
      return *this;
    }

  private:
    using EditablePolicy::equality;
    using EditablePolicy::convertToArrayType;
    using EditablePolicy::convertFromArrayType;
    using DirectionPolicy::incrementeArray;
    using DirectionPolicy::offset;

    ArrayType m_array;
    int m_index;
  };


#if 0



  template<size_t N>
  class Iterator<ScanDirection::Backward, Editable::True, EditionReference, N> {
  public:
  private:
    /* This code is UGLY, please do something. */
    Node getNode(int index) {
      if (m_index < 0) {
        // Special case: end iterator
        return Node();
      } else if (m_index == 0) {
        /* We can't keep a reference outside the scanned tree so we create
         * an edge case for the right most child: it's referenced by the parent
         * node. */
        return m_referencesArray[index].node().nextTree().previousNode();
      }
      return Node(m_references[index].node().block() + static_cast<int>(ScanDirection::Backward)());
    }
    void setNode(int index, Node node) {
      if (node.isUninitialized()) {
        // Special case: end iterator
        m_index = -2;
        return;
      }
      m_references[index] = EditionReference(Node(node.block() - static_cast<int>(ScanDirection::Backward)));
    }
    Node incrNode(Node node) { return node.previousTree(); }
  };

#endif
public:
  /* Scanner */

  template <typename DirectionPolicy, typename EditablePolicy, size_t N>
  class ChildrenScanner : private DirectionPolicy, private EditablePolicy {
  public:
    typedef typename EditablePolicy::NodeType NodeType;
    typedef std::array<NodeType ,N> ArrayType;

    ChildrenScanner(ArrayType array) : m_array(array) {}
    Iterator<DirectionPolicy, EditablePolicy, N> begin() const { return Iterator<DirectionPolicy, EditablePolicy, N>(convertToArrayType(firstElement(convertFromArrayType(m_array, offset())), offset()), 0); }
    Iterator<DirectionPolicy, EditablePolicy, N> end() const {
      std::array<Node, N> nodeArray = convertFromArrayType(m_array, offset());
      return Iterator<DirectionPolicy, EditablePolicy, N>(convertToArrayType(lastElement(nodeArray), offset()), lastIndex(nodeArray)); }

  protected:
    using EditablePolicy::convertFromArrayType;
    using EditablePolicy::convertToArrayType;
    using DirectionPolicy::firstElement;
    using DirectionPolicy::lastElement;
    using DirectionPolicy::offset;

    ArrayType m_array;

  private:
    int lastIndex(std::array<Node, N> array) const {
      uint8_t nbOfChildren = UINT8_MAX;
      for (size_t i = 0; i < N; i++) {
        nbOfChildren = std::min<uint8_t>(nbOfChildren, array[i].numberOfChildren());
      }
      return nbOfChildren;
    }

  };

  class NoEditablePolicy {
  public:
    typedef Node NodeType;
    template<size_t N> using ArrayType = std::array<NodeType, N>;
  protected:
    template <size_t N>
    bool equality(ArrayType<N> array0, int index0, ArrayType<N>array1, int index1) const { return (index0 != index1); }

    template<size_t N>
    std::array<Node, N> convertFromArrayType(ArrayType<N> array, int offset = 0) const { return array; }
    template<size_t N>
    ArrayType<N> convertToArrayType(std::array<Node, N> array, int offset = 0) const { return array; }
  };

  class EditablePolicy {
  public:
    typedef EditionReference NodeType;
    template<size_t N> using ArrayType = std::array<NodeType, N>;
  protected:
    template <size_t N>
    bool equality(ArrayType<N> array0, int index0, ArrayType<N>array1, int index1) const {
      for (size_t i = 0; i < N; i++) {
        if (array0[i].node() == array1[i].node()) {
          return false;
        }
      }
      return true;
    }

    /* Hack: we keep a reference to a block right before (or after) the
     * currenNode to handle cases where the current node is replaced by
     * another one. The assertion that the previous children aren't modified
     * ensure the validity of this hack. */

    template<size_t N>
    std::array<Node, N> convertFromArrayType(ArrayType<N> array, int offset = 0) const {
      return Array::MapAction<NodeType, Node, N>(array, &offset, [](NodeType reference, void * offset) { return Node(reference.node().block() + *static_cast<int *>(offset)); });
    }
    template<size_t N>
    ArrayType<N> convertToArrayType(std::array<Node, N> array, int offset = 0) const {
      return Array::MapAction<Node, NodeType, N>(array, &offset, [](Node node, void * offset) { return EditionReference(Node(node.block() - *static_cast<int *>(offset))); });
    }
  };

  class ForwardPolicy {
  protected:
    template<size_t N>
    std::array<Node, N> firstElement(std::array<Node, N> array) const { return Array::MapAction<Node, Node, N>(array, nullptr, [](Node node, void * context) { return node.nextNode(); }); }

    template<size_t N>
    std::array<Node, N> lastElement(std::array<Node, N> array) const { return Array::MapAction<Node, Node, N>(array, nullptr, [](Node node, void * context) { return node.nextTree(); }); }

    template<size_t N>
    std::array<Node, N> incrementeArray(std::array<Node, N> array) const { return Array::MapAction<Node, Node, N>(array, nullptr, [](Node node, void * context) { return node.nextTree(); }); }

    int offset() const { return 1; }
  };

  class BackwardPolicy {
  protected:
    template<size_t N>
    std::array<Node, N> firstElement(std::array<Node, N> array) const { return Array::MapAction<Node, Node, N>(array, nullptr, [](Node node, void * context) { return node.nextTree().previousNode(); }); }

    template<size_t N>
    std::array<Node, N> lastElement(std::array<Node, N> array) const { return Array::MapAction<Node, Node, N>(array, nullptr, [](Node node, void * context) { return Node(); }); }

    template<size_t N>
    std::array<Node, N> incrementeArray(std::array<Node, N> array) const { return Array::MapAction<Node, Node, N>(array, nullptr, [](Node node, void * context) { return node.previousTree(); }); }

    int offset() const { return -1; }
  };


  /* Scanner specialization */

  template <typename DirectionPolicy, typename EditablePolicy, size_t N>
  static ChildrenScanner<DirectionPolicy, EditablePolicy, N> Children(std::array<typename EditablePolicy::NodeType, N> array) { return ChildrenScanner<DirectionPolicy, EditablePolicy, N>(array); }

  /* Workaround: don't emit the undefined Children<ScanDirection::?, Editable::False, Node, N>
   * but fallback to Children<ScanDirection::?, Editable::False, const Node, N>. */
  //template <ScanDirection direction, Editable::False, const Node, N>
  //static ChildrenScanner<direction, Editable::False, const Node> Children(Node node) { return ChildrenScanner<direction, Editable::False, const Node>(node); }
};

class NodeIterator : public MultipleNodesIterator {
private:
  template <typename DirectionPolicy, typename EditablePolicy>
  class Iterator {
  public:
    Iterator(MultipleNodesIterator::Iterator<DirectionPolicy, EditablePolicy, 1> iterator) : m_iterator(iterator) {}
    std::pair<typename EditablePolicy::NodeType, int> operator*() { return std::pair(std::get<0>(*m_iterator)[0], std::get<1>(*m_iterator)); }
    bool operator!=(Iterator& it) { return m_iterator != it.m_iterator; }
    Iterator<DirectionPolicy, EditablePolicy> & operator++() {
      m_iterator.operator++();
      return *this;
    }
  private:
    MultipleNodesIterator::Iterator<DirectionPolicy, EditablePolicy, 1> m_iterator;
  };

public:
  template <typename DirectionPolicy, typename EditablePolicy>
  class ChildrenScanner {
  public:
    typedef typename EditablePolicy::NodeType NodeType;
    ChildrenScanner(NodeType node) : m_scanner(std::array<NodeType, 1>({node})) {}
    Iterator<DirectionPolicy, EditablePolicy> begin() const { return Iterator<DirectionPolicy, EditablePolicy>(m_scanner.begin()); }
    Iterator<DirectionPolicy, EditablePolicy>  end() const { return Iterator<DirectionPolicy, EditablePolicy>(m_scanner.end()); }

  protected:
    MultipleNodesIterator::ChildrenScanner<DirectionPolicy, EditablePolicy, 1> m_scanner;
  };

  template <typename DirectionPolicy, typename EditablePolicy>
  static ChildrenScanner<DirectionPolicy, EditablePolicy> Children(typename EditablePolicy::NodeType node) { return ChildrenScanner<DirectionPolicy, EditablePolicy>(node); }
};

typedef MultipleNodesIterator::ForwardPolicy Forward;
typedef MultipleNodesIterator::BackwardPolicy Backward;
typedef MultipleNodesIterator::NoEditablePolicy NoEditable;
typedef MultipleNodesIterator::EditablePolicy Editable;

}
#endif
