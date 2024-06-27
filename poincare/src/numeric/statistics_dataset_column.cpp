#include "statistics_dataset_column.h"

#include <assert.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

template <typename T>
TreeDatasetColumn<T>::TreeDatasetColumn(const Tree* e) : m_list(e) {
  assert(m_list->isList());
}

template <typename T>
T TreeDatasetColumn<T>::valueAtIndex(int index) const {
  /* TODO: this is inefficient since we approximate the Tree each time we need
   * to access an element. We should have two classes, one for general trees and
   * one already approximated with fast array access. */
  return Approximation::RootTreeToReal<T>(m_list->child(index));
}

template <typename T>
int TreeDatasetColumn<T>::length() const {
  return m_list->numberOfChildren();
}

template class TreeDatasetColumn<float>;
template class TreeDatasetColumn<double>;

}  // namespace Poincare::Internal
