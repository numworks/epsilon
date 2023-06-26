#ifndef POINCARE_MEMORY_NODE_ITERATOR_H
#define POINCARE_MEMORY_NODE_ITERATOR_H

#include <omgpj/array.h>

#include <algorithm>
#include <array>

#include "edition_reference.h"

namespace PoincareJ {

/* Usage
 *
  * Iterator forwards concomittantly through 2 nodes' children


  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs :
 MultipleNodesIterator::Children<Editable,
 2>(std::array<EditionReference, 2>({node0, node1}))) { EditionReference
 childOfNode0 = std::get<0>(indexedRefs)[0]; EditionReference childOfNode1 =
 std::get<0>(indexedRefs)[1]; int index = std::get<int>(indexedNode);
    ...
  }

 * */

class MultipleNodesIterator {
  /* Generic iterators, please choose:
   * - the editability of nodes: True or False
   * - the number of nodes we're iterating through
   *
   * For instance: ChildrenScanner<Editable, 2> is a scanner concomittantly
   * iterating through 2 nodes' children.
   *
   * Please note:
   * The scanning direction is always forward.
   * You can use the editable scan only for trees located in the editable pool.
   * When doing so you can only edit the children downstream (the children after
   * the current child).
   */

 protected:  // templates force us to define some protected classes firstA
  class ForwardPolicy {
   protected:
    template <size_t N>
    std::array<const Tree *, N> firstElement(
        std::array<const Tree *, N> array) const {
      return Array::MapAction<const Tree *, const Tree *, N>(
          array, nullptr,
          [](const Tree *node, void *context) { return node->nextNode(); });
    }

    template <size_t N>
    std::array<const Tree *, N> incrementeArray(
        std::array<const Tree *, N> array) const {
      return Array::MapAction<const Tree *, const Tree *, N>(
          array, nullptr,
          [](const Tree *node, void *context) { return node->nextTree(); });
    }

    int offset() const { return 1; }
  };

  /* Iterator */

  template <typename EditablePolicy, size_t N>
  class Iterator : private ForwardPolicy, private EditablePolicy {
   public:
    typedef typename EditablePolicy::NodeType NodeType;
    typedef std::array<NodeType, N> ArrayType;

    Iterator(ArrayType array, int index) : m_array(array), m_index(index) {}
    std::pair<ArrayType, int> operator*() {
      return std::pair(
          convertToArrayType(convertFromArrayType(m_array, offset())), m_index);
    }
    bool operator!=(Iterator<EditablePolicy, N> &it) {
      return equality(m_array, m_index, it.m_array, it.m_index);
    }
    Iterator<EditablePolicy, N> &operator++() {
      m_array = convertToArrayType(
          incrementeArray(convertFromArrayType(m_array, offset())), offset());
      m_index++;
      return *this;
    }

   private:
    using EditablePolicy::convertFromArrayType;
    using EditablePolicy::convertToArrayType;
    using EditablePolicy::equality;
    using ForwardPolicy::incrementeArray;
    using ForwardPolicy::offset;

    ArrayType m_array;
    int m_index;
  };

 public:
  /* Scanner */

  template <typename EditablePolicy, size_t N>
  class ChildrenScanner : private ForwardPolicy, private EditablePolicy {
   public:
    typedef typename EditablePolicy::NodeType NodeType;
    typedef std::array<NodeType, N> ArrayType;

    ChildrenScanner(ArrayType array) : m_array(array) {}
    Iterator<EditablePolicy, N> begin() const {
      return Iterator<EditablePolicy, N>(
          convertToArrayType(firstElement(convertFromArrayType(m_array)),
                             offset()),
          0);
    }
    Iterator<EditablePolicy, N> end() const {
      return Iterator<EditablePolicy, N>(
          m_array, endIndex(convertFromArrayType(m_array)));
    }

   protected:
    using EditablePolicy::convertFromArrayType;
    using EditablePolicy::convertToArrayType;
    using EditablePolicy::endIndex;
    using ForwardPolicy::firstElement;
    using ForwardPolicy::offset;

    ArrayType m_array;
  };

  template <typename EditablePolicy, size_t N>
  static ChildrenScanner<EditablePolicy, N> Children(
      std::array<typename EditablePolicy::NodeType, N> array) {
    return ChildrenScanner<EditablePolicy, N>(array);
  }

  /* Policies */

  class NoEditablePolicy {
   public:
    typedef const Tree *NodeType;
    template <size_t N>
    using ArrayType = std::array<NodeType, N>;
    template <size_t N>
    int endIndex(std::array<NodeType, N> array) const {
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
    std::array<NodeType, N> convertFromArrayType(ArrayType<N> array,
                                                 int offset = 0) const {
      return array;
    }
    template <size_t N>
    ArrayType<N> convertToArrayType(std::array<NodeType, N> array,
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
    int endIndex(std::array<const Tree *, N> array) const {
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
    std::array<const Tree *, N> convertFromArrayType(ArrayType<N> array,
                                                     int offset = 0) const {
      return Array::MapAction<NodeType, const Tree *, N>(
          array, &offset, [](NodeType reference, void *offset) -> const Tree * {
            return Tree::FromBlocks(reference->block() +
                                    *static_cast<int *>(offset));
          });
    }
    template <size_t N>
    ArrayType<N> convertToArrayType(std::array<const Tree *, N> array,
                                    int offset = 0) const {
      return Array::MapAction<const Tree *, NodeType, N>(
          array, &offset, [](const Tree *node, void *offset) {
            return node ? EditionReference(Tree::FromBlocks(
                              const_cast<Tree *>(node)->block() -
                              *static_cast<int *>(offset)))
                        : EditionReference();
          });
    }
  };
};

class NodeIterator : public MultipleNodesIterator {
  /* Ensure lighter syntax for one node iterator */

 private:
  template <typename EditablePolicy>
  class Iterator {
   public:
    Iterator(MultipleNodesIterator::Iterator<EditablePolicy, 1> iterator)
        : m_iterator(iterator) {}
    std::pair<typename EditablePolicy::NodeType, int> operator*() {
      return std::pair((*m_iterator).first[0], (*m_iterator).second);
    }
    bool operator!=(Iterator &it) { return m_iterator != it.m_iterator; }
    Iterator<EditablePolicy> &operator++() {
      m_iterator.operator++();
      return *this;
    }

   private:
    MultipleNodesIterator::Iterator<EditablePolicy, 1> m_iterator;
  };

 public:
  template <typename EditablePolicy>
  class ChildrenScanner {
   public:
    typedef typename EditablePolicy::NodeType NodeType;
    ChildrenScanner(NodeType node)
        : m_scanner(std::array<NodeType, 1>({node})) {}
    Iterator<EditablePolicy> begin() const {
      return Iterator<EditablePolicy>(m_scanner.begin());
    }
    Iterator<EditablePolicy> end() const {
      return Iterator<EditablePolicy>(m_scanner.end());
    }

   protected:
    MultipleNodesIterator::ChildrenScanner<EditablePolicy, 1> m_scanner;
  };

  template <typename EditablePolicy>
  static ChildrenScanner<EditablePolicy> Children(
      typename EditablePolicy::NodeType node) {
    return ChildrenScanner<EditablePolicy>(node);
  }
};

typedef MultipleNodesIterator::NoEditablePolicy NoEditable;
typedef MultipleNodesIterator::EditablePolicy Editable;

}  // namespace PoincareJ

#endif
