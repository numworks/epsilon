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
  const Internal::Tree* p =
      t->nextNode() + i * Internal::TypeBlock::NumberOfMetaBlocks(
                              Internal::Type::PointOfInterest);
  return reinterpret_cast<
      const Internal::CustomTypeStructs::PointOfInterestNode*>(p + 1);
}

Internal::CustomTypeStructs::PointOfInterestNode* pointInTree(Internal::Tree* t,
                                                              int i) {
  return const_cast<Internal::CustomTypeStructs::PointOfInterestNode*>(
      pointInTree(const_cast<const Internal::Tree*>(t), i));
}

}  // namespace

void PointsOfInterestList::moveToStack() {
  assert(!m_stackList);
  assert(!isUninitialized());
  assert(Internal::TreeStack::SharedTreeStack->numberOfTrees() == 0);
  m_stackList = m_poolList.tree()->cloneTree();
  m_poolList = {};
}

void PointsOfInterestList::moveToPool() {
  assert(isUninitialized());
  assert(m_stackList);
  m_poolList = API::JuniorPoolHandle::Builder(m_stackList);
  m_stackList = nullptr;
  assert(Internal::TreeStack::SharedTreeStack->numberOfTrees() == 0);
}

int PointsOfInterestList::numberOfPoints() const {
  assert(!m_stackList);
  if (m_poolList.isUninitialized()) {
    return 0;
  }
  const Internal::Tree* t = m_poolList.tree();
  assert(t->isList());
  return t->numberOfChildren();
}

PointOfInterest PointsOfInterestList::pointAtIndex(int i) const {
  assert(!m_stackList);
  assert(0 <= i && i < numberOfPoints());
  /* The list is supposed to only contain PointOfInterestNodes, take advantage
   * of this to fetch the child with pointer arithmetic instead of walking the
   * tree. */
  PointOfInterest result;
  assert(sizeof(result) ==
         sizeof(Internal::CustomTypeStructs::PointOfInterestNode));
  memcpy(&result, pointInTree(m_poolList.tree(), i), sizeof(result));
  return result;
}

void PointsOfInterestList::sort() {
  moveToStack();
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
      m_stackList, m_stackList->numberOfChildren());
  moveToPool();
}

void PointsOfInterestList::filterOutOfBounds(double start, double end) {
  assert(!m_stackList);
  assert(!isUninitialized());
  m_stackList = Internal::List::PushEmpty();
  for (const Internal::Tree* child : m_poolList.tree()->children()) {
    Internal::CustomTypeStructs::PointOfInterestNode p = *reinterpret_cast<
        const Internal::CustomTypeStructs::PointOfInterestNode*>(child + 1);
    if (start <= p.abscissa && p.abscissa <= end) {
      Internal::NAry::AddChild(m_stackList, child->cloneTree());
    }
  }
  m_poolList = {};
  moveToPool();
}

void PointsOfInterestList::append(PointOfInterest p) {
  // FIXME This can overflow the pool and stack
  moveToStack();
  Internal::Tree* newPoint =
      Internal::TreeStack::SharedTreeStack->pushPointOfInterest(
          p.abscissa, p.ordinate, p.data, static_cast<uint8_t>(p.interest),
          p.inverted, p.subCurveIndex);
  Internal::NAry::AddChild(m_stackList, newPoint);
  moveToPool();
}

}  // namespace Poincare
