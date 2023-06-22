#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

#include <omgpj/array.h>

#include <algorithm>
#include <array>

#include "edition_reference.h"

namespace PoincareJ {

#if POINCARE_JUNIOR_BACKWARD_SCAN
/* Backward edition has issues with the offset being unreliable as the node is
 * edited. In addition, we try to reduce our usage of previousTree and most
 * Backward scan are currently unnecessary.
 * TODO: Fix Backward scan if needed, delete otherwise. */
#endif

/* Usage
 *
 * Iterate backwards through on node's children:

  for (const std::pair<Node*, int> indexedNode :
 NodeIterator::Children<Backward, NoEditable>(node)) { Node* child =
 std::get<Node*>(indexedNode); int index = std::get<int>(indexedNode);
    ...
  }

  * Iterator forwards concomittantly through 2 nodes' children


  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs :
 MultipleNodesIterator::Children<Forward, Editable,
 2>(std::array<EditionReference, 2>({node0, node1}))) { EditionReference
 childOfNode0 = std::get<0>(indexedRefs)[0]; EditionReference childOfNode1 =
 std::get<0>(indexedRefs)[1]; int index = std::get<int>(indexedNode);
    ...
  }

 * */

class MultipleNodesIterator {
  /* Generic iterators, please choose:
   * - the scanning direction: forward or backward
   * - the editability of nodes: True or False
   * - the number of nodes we're iterating through
   *
   * For instance: ChildrenScanner<Forward, Editable, 2> is a scanner
   * concomittantly iterating through 2 nodes' children.
   *
   * Please note:
   * You can use the editable scan only for trees located in the editable pool.
   * When doing so you can only edit the children downstream (the children after
   * the current child if you're going forwards and the children before the
   * current child if you're going backwards).
   */

 protected:  // templates force us to define some protected classes firstA
  /* Iterator */

  template <typename DirectionPolicy, typename EditablePolicy, size_t N>
  class Iterator : private DirectionPolicy, private EditablePolicy {
   public:
    typedef typename EditablePolicy::NodeType NodeType;
    typedef std::array<NodeType, N> ArrayType;

    Iterator(ArrayType array, int index) : m_array(array), m_index(index) {}
    std::pair<ArrayType, int> operator*() {
      return std::pair(
          convertToArrayType(convertFromArrayType(m_array, offset())), m_index);
    }
    bool operator!=(Iterator<DirectionPolicy, EditablePolicy, N> &it) {
      return equality(m_array, m_index, it.m_array, it.m_index);
    }
    Iterator<DirectionPolicy, EditablePolicy, N> &operator++() {
      m_array = convertToArrayType(
          incrementeArray(convertFromArrayType(m_array, offset())), offset());
      m_index++;
      return *this;
    }

   private:
    using DirectionPolicy::incrementeArray;
    using DirectionPolicy::offset;
    using EditablePolicy::convertFromArrayType;
    using EditablePolicy::convertToArrayType;
    using EditablePolicy::equality;

    ArrayType m_array;
    int m_index;
  };

 public:
  /* Scanner */

  template <typename DirectionPolicy, typename EditablePolicy, size_t N>
  class ChildrenScanner : private DirectionPolicy, private EditablePolicy {
   public:
    typedef typename EditablePolicy::NodeType NodeType;
    typedef std::array<NodeType, N> ArrayType;

    ChildrenScanner(ArrayType array) : m_array(array) {}
    Iterator<DirectionPolicy, EditablePolicy, N> begin() const {
      return Iterator<DirectionPolicy, EditablePolicy, N>(
          convertToArrayType(firstElement(convertFromArrayType(m_array)),
                             offset()),
          0);
    }
    Iterator<DirectionPolicy, EditablePolicy, N> end() const {
      return Iterator<DirectionPolicy, EditablePolicy, N>(
          m_array, endIndex(convertFromArrayType(m_array)));
    }

   protected:
    using DirectionPolicy::firstElement;
    using DirectionPolicy::offset;
    using EditablePolicy::convertFromArrayType;
    using EditablePolicy::convertToArrayType;
    using EditablePolicy::endIndex;

    ArrayType m_array;
  };

  template <typename DirectionPolicy, typename EditablePolicy, size_t N>
  static ChildrenScanner<DirectionPolicy, EditablePolicy, N> Children(
      std::array<typename EditablePolicy::NodeType, N> array) {
    return ChildrenScanner<DirectionPolicy, EditablePolicy, N>(array);
  }

  /* Policies */

  class NoEditablePolicy {
   public:
    typedef Node *NodeType;
    template <size_t N>
    using ArrayType = std::array<NodeType, N>;
    template <size_t N>
    int endIndex(std::array<Node *, N> array) const {
      uint8_t nbOfChildren = UINT8_MAX;
      for (size_t i = 0; i < N; i++) {
        nbOfChildren =
            std::min<uint8_t>(nbOfChildren, array[i]->numberOfChildren());
      }
      return nbOfChildren;
    }

   protected:
    template <size_t N>
    bool equality(ArrayType<N> array0, int index0, ArrayType<N> array1,
                  int index1) const {
      return (index0 != index1);
    }
    template <size_t N>
    std::array<Node *, N> convertFromArrayType(ArrayType<N> array,
                                               int offset = 0) const {
      return array;
    }
    template <size_t N>
    ArrayType<N> convertToArrayType(std::array<Node *, N> array,
                                    int offset = 0) const {
      return array;
    }
  };

  class EditablePolicy {
   public:
    typedef EditionReference NodeType;
    template <size_t N>
    using ArrayType = std::array<NodeType, N>;

   protected:
    /* Special case for the end index:
     * endIndex = -1 to trigger a special case on equality and recompute the
     * minimal number of children of all node in the array. This has to be
     * updated at each step since children might have been inserted or deleted.
     */
    template <size_t N>
    int endIndex(std::array<Node *, N> array) const {
      return -1;
    }
    template <size_t N>
    bool equality(ArrayType<N> array0, int index0, ArrayType<N> array1,
                  int index1) const {
      // the end element (idnex = -1) is always the second
      assert(index0 >= 0);
      if (index1 < 0) {
        // Recompute the minimal number of children
        index1 = NoEditablePolicy().endIndex(convertFromArrayType(array1));
      }
      return index0 != index1;
    }

    /* Hack: we keep a reference to a block right before (or after) the
     * currenNode to handle cases where the current node is replaced by
     * another one. The assertion that the previous children aren't modified
     * ensure the validity of this hack. */

    template <size_t N>
    std::array<Node *, N> convertFromArrayType(ArrayType<N> array,
                                               int offset = 0) const {
      return Array::MapAction<NodeType, Node *, N>(
          array, &offset, [](NodeType reference, void *offset) {
            return Node::FromBlocks(reference.block() +
                                    *static_cast<int *>(offset));
          });
    }
    template <size_t N>
    ArrayType<N> convertToArrayType(std::array<Node *, N> array,
                                    int offset = 0) const {
      return Array::MapAction<Node *, NodeType, N>(
          array, &offset, [](Node *node, void *offset) {
            return node ? EditionReference(Node::FromBlocks(
                              node->block() - *static_cast<int *>(offset)))
                        : EditionReference();
          });
    }
  };

  class ForwardPolicy {
   protected:
    template <size_t N>
    std::array<Node *, N> firstElement(std::array<Node *, N> array) const {
      return Array::MapAction<Node *, Node *, N>(
          array, nullptr,
          [](Node *node, void *context) { return node->nextNode(); });
    }

    template <size_t N>
    std::array<Node *, N> incrementeArray(std::array<Node *, N> array) const {
      return Array::MapAction<Node *, Node *, N>(
          array, nullptr,
          [](Node *node, void *context) { return node->nextTree(); });
    }

    int offset() const { return 1; }
  };

#if POINCARE_JUNIOR_BACKWARD_SCAN
  class BackwardPolicy {
   protected:
    template <size_t N>
    std::array<Node *, N> firstElement(std::array<Node *, N> array) const {
      return Array::MapAction<Node *, Node *, N>(
          array, nullptr, [](Node *node, void *context) {
            return node->childAtIndex(node->numberOfChildren() - 1);
          });
    }

    template <size_t N>
    std::array<Node *, N> incrementeArray(std::array<Node *, N> array) const {
      return Array::MapAction<Node *, Node *, N>(
          array, nullptr,
          [](Node *node, void *context) { return node->previousTree(); });
    }

    int offset() const { return -1; }
  };
#endif
};

class NodeIterator : public MultipleNodesIterator {
  /* Ensure lighter syntax for one node iterator */

 private:
  template <typename DirectionPolicy, typename EditablePolicy>
  class Iterator {
   public:
    Iterator(MultipleNodesIterator::Iterator<DirectionPolicy, EditablePolicy, 1>
                 iterator)
        : m_iterator(iterator) {}
    std::pair<typename EditablePolicy::NodeType, int> operator*() {
      return std::pair((*m_iterator).first[0], (*m_iterator).second);
    }
    bool operator!=(Iterator &it) { return m_iterator != it.m_iterator; }
    Iterator<DirectionPolicy, EditablePolicy> &operator++() {
      m_iterator.operator++();
      return *this;
    }

   private:
    MultipleNodesIterator::Iterator<DirectionPolicy, EditablePolicy, 1>
        m_iterator;
  };

 public:
  template <typename DirectionPolicy, typename EditablePolicy>
  class ChildrenScanner {
   public:
    typedef typename EditablePolicy::NodeType NodeType;
    ChildrenScanner(NodeType node)
        : m_scanner(std::array<NodeType, 1>({node})) {}
    Iterator<DirectionPolicy, EditablePolicy> begin() const {
      return Iterator<DirectionPolicy, EditablePolicy>(m_scanner.begin());
    }
    Iterator<DirectionPolicy, EditablePolicy> end() const {
      return Iterator<DirectionPolicy, EditablePolicy>(m_scanner.end());
    }

   protected:
    MultipleNodesIterator::ChildrenScanner<DirectionPolicy, EditablePolicy, 1>
        m_scanner;
  };

  template <typename DirectionPolicy, typename EditablePolicy>
  static ChildrenScanner<DirectionPolicy, EditablePolicy> Children(
      typename EditablePolicy::NodeType node) {
    return ChildrenScanner<DirectionPolicy, EditablePolicy>(node);
  }
};

typedef MultipleNodesIterator::ForwardPolicy Forward;
#if POINCARE_JUNIOR_BACKWARD_SCAN
typedef MultipleNodesIterator::BackwardPolicy Backward;
#endif
typedef MultipleNodesIterator::NoEditablePolicy NoEditable;
typedef MultipleNodesIterator::EditablePolicy Editable;

}  // namespace PoincareJ

#endif
