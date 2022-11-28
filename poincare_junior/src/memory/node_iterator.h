#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

#include <array>
#include "edition_reference.h"
#include <utils/enums.h>

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

  template <ScanDirection direction, Editable isEditable, typename T, size_t N>
  class Iterator {
  public:
    Iterator(std::array<T,N> array, int index) : m_array(array), m_index(index) {}
    std::pair<std::array<T,N>, int> operator*();
    bool operator!=(Iterator& it);
    Iterator<direction, isEditable, T, N> & operator++();
  private:
    std::array<T,N> m_array;
    int m_index;
  };

  /* Iterator specialization */

#if 0
  template <ScanDirection direction, size_t N>
  class Iterator<direction, Editable::False, const Node, N> {
  public:
    std::pair<std::array<const Node,N>, int> operator*() { return std::pair(m_array, m_index); }
    bool operator!=(Iterator<direction, Editable::False, const Node, N>& it) { return (m_index != it.m_index); }
    Iterator<direction, Editable::False, const Node, N> & operator++() {
      for (Node & node : m_array) {
        node = incrNode(node);
      }
      m_index++;:
      return *this;
    }
  };

  template<size_t N>
  class Iterator<ScanDirection::Forward, Editable::False, const Node, N> {
  private:
    Node incrNode(Node node) { return node.nextTree(); }
  };

  template<size_t N>
  class Iterator<ScanDirection::Backward, Editable::False, const Node, N> {
  private:
    Node incrNode(Node node) { return node.previousTree(); }
  };


  template<ScanDirection direction, size_t N>
  class Iterator<direction, Editable::True, EditionReference, N> {
  public:
    std::pair<std::array<EditionReference,N>, int> operator*() { return std::pair(m_array, m_index); }
    bool operator!=(Iterator<direction, Editable::True, EditionReference, N>& it) {
      for (size_t i = 0; i < N; i++) {
        if (getNode(i) == it.getNode(i)) {
          return false;
        }
      }
      return true;
    }
    Iterator<direction, Editable::True, EditionReference, N> & operator++() {
      for (size_t i = 0; i < N; i++) {
        setNode(i, incrNode(getNode(i)));
      }
      m_index++;
      return *this;
    }
  private:
    /* Hack: we keep a reference to a block right before (or after) the
     * currenNode to handle cases where the current node is replaced by
     * another one. The assertion that the previous children aren't modified
     * ensure the validity of this hack. */
    Node getNode(int index);
    void setNode(int index, Node node);
    Node incrNode(Node node);
  };

  template<size_t N>
  class Iterator<ScanDirection::Forward, Editable::True, EditionReference, N> {
  public:
    Iterator<ScanDirection::Forward, Editable::True, EditionReference, N>() : m_referencesArray(), m_index(0) {
      for (size_t i = 0; i < N; i++) {
        setNode(i, m_referencesArray[i]);
      }
    }

  private:
    Node getNode(int index) { return Node(m_referencesArray[index].node().block() + static_cast<int>(ScanDirection::Forward)()); }
    void setNode(int index, Node node) { m_referencesArray[index] = EditionReference(Node(node.block() - static_cast<int>(ScanDirection::Forward))); }
    Node incrNode(Node node) { return node.nextTree(); }
  };

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

  template <ScanDirection direction, Editable isEditable, typename T, size_t N>
  class ChildrenScanner {
  public:
    ChildrenScanner(std::array<T,N> array) : m_array(array) {}
    Iterator<direction,isEditable,T,N> begin() const;
    Iterator<direction,isEditable,T,N>  end() const;

  protected:
    using Action = T (*)(T);
    std::array<T,N> mapAction(Action action) {
      std::array<T,N> newArray;
      for (size_t i = 0; i < N; i++) {
        newArray[i] = action(m_array[i]);
      }
      return newArray;
    }
    std::array<T,N> m_array;
  };

  /* Scanner specialization */

#if 0
  template <ScanDirection direction, size_t N>
  static Iterator<direction,Editable::False,const Node,N> ConstEndIterator(std::array<const Node,N> array) {
    uint8_t nbOfChildren = UINT8_MAX;
    for (size_t i = 0; i < N; i++) {
      nbOfChildren = std::min(nbOfChildren, array[i].numberOfChildren());
    }
    return Iterator<direction,Editable::False,const Node,N>(
        std::array<const Node, N>(),
        nbOfChildren
      );
  }

  template <size_t N>
  class ChildrenScanner<ScanDirection::Forward, Editable::False, const Node, N> {
  public:
    Iterator<ScanDirection::Forward,Editable::False,const Node,N>  begin() const {
      return Iterator<ScanDirection::Forward,Editable::False,const Node,N>(
          mapAction([](const Node node) { return node.nextNode(); }),
          0
        );
    }
    Iterator<ScanDirection::Forward,Editable::False,const Node,N> end() const { return ConstEndIterator<ScanDirection::Forward, N>(m_array); }
  private:
    std::array<const Node,N> m_array;
  };

  template <size_t N>
  class ChildrenScanner<ScanDirection::Backward, Editable::False, const Node, N> {
  public:
    Iterator<ScanDirection::Forward,Editable::False,const Node,N>  begin() const {
      return Iterator<ScanDirection::Forward,Editable::False,const Node,N>(
          mapAction([](const Node node) { return node.nextTree().previousNode(); }),
          0
        );
    }
  };

  template <size_t N>
  class ChildrenScanner<ScanDirection::Forward, Editable::True, EditionReference, N> {
  public:
    Iterator<ScanDirection::Forward,Editable::True,EditionReference,N>  begin() const {
      return Iterator<ScanDirection::Forward,Editable::True,EditionReference,N>(
          mapAction([](const Node node) { return node.nextNode(); }),
          0
        );
    }
    Iterator<ScanDirection::Forward,Editable::True,EditionReference,N> end() const {
      return Iterator<ScanDirection::Forward,Editable::True,EditionReference,N>(
          mapAction([](const Node node) { return node.nextTree(); }),
          0
        );
    }
  };

  template <size_t N>
  class ChildrenScanner<ScanDirection::Backward, Editable::True, EditionReference, N> {
  public:
    Iterator<ScanDirection::Backward,Editable::True,EditionReference,N>  begin() const {
      return Iterator<ScanDirection::Forward,Editable::True,EditionReference,N>(
          mapAction([](const Node node) { return node; }),
          0
        );
    }
    Iterator<ScanDirection::Backward,Editable::True,EditionReference,N> end() const {
      return Iterator<ScanDirection::Forward,Editable::True,EditionReference,N>(
          std::array<EditionReference, N>(),
          -1
        );
    }
  };
#endif
  template <ScanDirection direction, Editable isEditable, typename T, size_t N>
  static ChildrenScanner<direction, isEditable, T, N> Children(std::array<T,N> array) { return ChildrenScanner<direction, isEditable, T, N>(array); }

  /* Workaround: don't emit the undefined Children<ScanDirection::?, Editable::False, Node, N>
   * but fallback to Children<ScanDirection::?, Editable::False, const Node, N>. */
  //template <ScanDirection direction, Editable::False, const Node, N>
  //static ChildrenScanner<direction, Editable::False, const Node> Children(Node node) { return ChildrenScanner<direction, Editable::False, const Node>(node); }
};


class NodeIterator : public MultipleNodesIterator {
private:
  template <ScanDirection direction, Editable isEditable, typename T>
  class Iterator {
  public:
    Iterator(MultipleNodesIterator::Iterator<direction, isEditable, T, 1> iterator) : m_iterator(iterator) {}
    std::pair<T, int> operator*() { return std::pair(std::get<0>(*m_iterator)[0], std::get<1>(*m_iterator)); }
    bool operator!=(Iterator& it) { return m_iterator != it.m_iterator; }
    Iterator<direction, isEditable, T> & operator++() {
      m_iterator.operator++();
      return *this;
    }
  private:
    MultipleNodesIterator::Iterator<direction, isEditable, T, 1> m_iterator;
  };
public:
  template <ScanDirection direction, Editable isEditable, typename T>
  class ChildrenScanner {
  public:
    ChildrenScanner(T node) : m_scanner(std::array<T,1>({node})) {}
    Iterator<direction,isEditable,T> begin() const { return Iterator<direction,isEditable,T>(m_scanner.begin()); }
    Iterator<direction,isEditable,T>  end() const { return Iterator<direction,isEditable,T>(m_scanner.end()); }

  protected:
    MultipleNodesIterator::ChildrenScanner<direction, isEditable, T, 1> m_scanner;
  };

  template <ScanDirection direction, Editable isEditable, typename T>
  static ChildrenScanner<direction, isEditable, T> Children(T node) { return ChildrenScanner<direction, isEditable, T>(node); }
};

/* Specializations MultipleNodesIterator::ChildrenScanner */

#if 0
template <ScanDirection direction, size_t N>
MultipleNodesIterator::Iterator<direction, Editable::False, const Node, N> MultipleNodesIterator::ChildrenScanner<direction, Editable::False, const Node, N>::end() const {
  uint8_t nbOfChildren = UINT8_MAX;
  for (size_t i = 0; i < N; i++) {
    nbOfChildren = std::min(nbOfChildren, m_array[i].numberOfChildren());
  }
  return Iterator<direction ,Editable::False,const Node, N>(
      std::array<const Node, N>(),
      nbOfChildren
    );
}

template <ScanDirection direction, Editable isEditable, typename T, size_t N>
MultipleNodesIterator::Iterator<direction,isEditable,T,N> MultipleNodesIterator::ChildrenScanner<direction,isEditable,T,N>::begin() const {
  return Iterator<direction,isEditable, T,N>(
      std::array<T, N>(),
      23
      );
 }

template <size_t N>
MultipleNodesIterator::Iterator<ScanDirection::Forward,Editable::False,const Node,N> MultipleNodesIterator::ChildrenScanner<ScanDirection::Backward, Editable::False, const Node, N>::begin() const {
  return Iterator<ScanDirection::Forward,Editable::False,const Node,N>(
      mapAction([](const Node node) { return node.nextTree().previousNode(); }),
      0
      );
}

/* Specializations MultipleNodesIterator::Iterator */

template <ScanDirection direction, size_t N>
std::pair<std::array<const Node,N> MultipleNodesIterator::Iterator<direction, Editable::False, const Node, N>::operator*() { return std::pair(m_array, m_index); }

template <ScanDirection direction, size_t N>
bool MultipleNodesIterator::Iterator<direction, Editable::False, const Node, N>::operator!=(Iterator<direction, Editable::False, const Node, N>& it) { return (m_index != it.m_index); }

template <ScanDirection direction, size_t N>
Iterator<direction, Editable::False, const Node, N> & MultipleNodesIterator::Iterator<direction, Editable::False, const Node, N>::operator++() {
  for (Node & node : m_array) {
    node = incrNode(node);
  }
  m_index++;
  return *this;
}
#endif
}
#endif
