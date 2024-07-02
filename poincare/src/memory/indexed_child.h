#ifndef POINCARE_MEMORY_INDEXED_CHILD_H
#define POINCARE_MEMORY_INDEXED_CHILD_H

#include <assert.h>
#include <stdint.h>

namespace Poincare::Internal {

class Tree;

/* Helper class to associate a childIndex to a Tree used in iterations */

template <typename T>  // T is Tree* or const Tree*
struct _IndexedChildBase {
  _IndexedChildBase(T child) : m_child(nullptr), index(0) {
    // Used as the end of the iterators
    assert(child == nullptr);
  }
  _IndexedChildBase(T child, int index) : m_child(child), index(index) {}

  bool operator==(const _IndexedChildBase<T>& other) const = default;
  bool operator!=(const _IndexedChildBase<T>& other) const = default;
  T operator->() const { return m_child; }
  operator T() { return m_child; }

  T m_child;
  uint16_t index;
};

template <typename T>  // T is Tree* or const Tree*
struct IndexedChild;

template <>
struct IndexedChild<Tree*> : public _IndexedChildBase<Tree*> {
  using _IndexedChildBase::_IndexedChildBase;
};
template <>
struct IndexedChild<const Tree*> : public _IndexedChildBase<const Tree*> {
  using _IndexedChildBase::_IndexedChildBase;
  // Allow a IndexedChild<Tree*> to be turned into an IndexedChild<const Tree*>
  IndexedChild(IndexedChild<Tree*> t) : _IndexedChildBase(t.m_child, t.index) {}
};

}  // namespace Poincare::Internal
#endif
