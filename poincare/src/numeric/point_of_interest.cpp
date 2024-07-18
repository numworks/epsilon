#include <omg/list.h>
#include <poincare/numeric/point_of_interest.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/memory/n_ary.h>

namespace Poincare {

namespace {

const Internal::CustomTypeStructs::PointOfInterestNode* pointInTree(
    const Internal::Tree* t, int i) {
  assert(t->isList());
  assert(0 <= i && i < t->numberOfChildren());
  const Internal::CustomTypeStructs::PointOfInterestNode* p =
      reinterpret_cast<const Internal::CustomTypeStructs::PointOfInterestNode*>(
          t->nextNode()) +
      i;
  assert(reinterpret_cast<const Internal::Tree*>(p) == t->child(i));
  return p;
}

Internal::CustomTypeStructs::PointOfInterestNode* pointInTree(Internal::Tree* t,
                                                              int i) {
  return const_cast<Internal::CustomTypeStructs::PointOfInterestNode*>(
      pointInTree(const_cast<const Internal::Tree*>(t), i));
}

}  // namespace

int PointsOfInterestList::numberOfPoints() const {
  if (m_list.isUninitialized()) {
    return 0;
  }
  const Internal::Tree* t = m_list.tree();
  assert(t->isList());
  return t->numberOfChildren();
}

PointOfInterest PointsOfInterestList::pointAtIndex(int i) const {
  assert(0 <= i && i < numberOfPoints());
  /* The list is supposed to only contain PointOfInterestNodes, take advantage
   * of this to fetch the child with pointer arithmetic instead of walking the
   * tree. */
  PointOfInterest result;
  assert(sizeof(result) ==
         sizeof(Internal::CustomTypeStructs::PointOfInterestNode) - 1);
  memcpy(&result, pointInTree(m_list.tree(), i), sizeof(result));
  return result;
}

void PointsOfInterestList::sort() {
  Internal::Tree* editableList = m_list.tree()->cloneTree();
  OMG::List::Sort(
      [](int i, int j, void* ctx, int n) {
        Internal::Tree* l = static_cast<Internal::Tree*>(ctx);
        Internal::CustomTypeStructs::PointOfInterestNode temp =
            *pointInTree(l, i);
        *pointInTree(l, i) = *pointInTree(l, j);
        *pointInTree(l, j) = temp;
      },
      [](int i, int j, void* ctx, int n) {
        Internal::Tree* l = static_cast<Internal::Tree*>(ctx);
        return pointInTree(l, i)->abscissa >= pointInTree(l, j)->abscissa;
      },
      editableList, numberOfPoints());
  m_list = API::JuniorPoolHandle::Builder(editableList);
}

void PointsOfInterestList::filterOutOfBounds(double start, double end) {
  Internal::Tree* editableList = Internal::List::PushEmpty();
  for (const Internal::Tree* child : m_list.tree()->children()) {
    Internal::CustomTypeStructs::PointOfInterestNode p = *reinterpret_cast<
        const Internal::CustomTypeStructs::PointOfInterestNode*>(child);
    if (start <= p.abscissa && p.abscissa <= end) {
      Internal::NAry::AddChild(editableList, child->cloneTree());
    }
  }
  m_list = API::JuniorPoolHandle::Builder(editableList);
}

}  // namespace Poincare
