#include <omg/list.h>
#include <poincare/numeric/point_of_interest.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

namespace Poincare {

namespace {

const Internal::Tree* pointAddressInTree(const Internal::Tree* t, int i) {
  assert(t->isList());
  assert(0 <= i && i < t->numberOfChildren());
  /* The list is supposed to only contain PointOfInterestNodes, take advantage
   * of this to fetch the child with pointer arithmetic instead of walking the
   * tree. */
  return t->nextNode() + i * Internal::TypeBlock::NumberOfMetaBlocks(
                                 Internal::Type::PointOfInterest);
}

Internal::Tree* pointAddressInTree(Internal::Tree* t, int i) {
  return const_cast<Internal::Tree*>(
      pointAddressInTree(const_cast<const Internal::Tree*>(t), i));
}

PointOfInterest pointFromTree(const Internal::Tree* t) {
  PointOfInterest result;
  assert(sizeof(result) ==
         sizeof(Internal::CustomTypeStructs::PointOfInterestNode));
  memcpy(&result, t + 1 /* skip the TypeBlock */, sizeof(result));
  return result;
}

}  // namespace

int PointsOfInterestList::numberOfPoints() const {
  assert(isStashEmpty());
  if (m_list.isUninitialized()) {
    return 0;
  }
  const Internal::Tree* t = m_list.tree();
  assert(t->isList());
  return t->numberOfChildren();
}

PointOfInterest PointsOfInterestList::pointAtIndex(int i) const {
  assert(isStashEmpty());
  assert(0 <= i && i < numberOfPoints());
  return pointFromTree(pointAddressInTree(m_list.tree(), i));
}

void PointsOfInterestList::sort() {
  assert(isStashEmpty());
  Internal::Tree* editableList = m_list.tree()->cloneTree();
  OMG::List::Sort(
      [](int i, int j, void* ctx, int n) {
        Internal::Tree* l = static_cast<Internal::Tree*>(ctx);
        pointAddressInTree(l, i)->swapWithTree(pointAddressInTree(l, j));
      },
      [](int i, int j, void* ctx, int n) {
        Internal::Tree* l = static_cast<Internal::Tree*>(ctx);
        return pointFromTree(pointAddressInTree(l, i)).abscissa >=
               pointFromTree(pointAddressInTree(l, j)).abscissa;
      },
      editableList, numberOfPoints());
  m_list = API::JuniorPoolHandle::Builder(editableList);
}

void PointsOfInterestList::filterOutOfBounds(double start, double end) {
  assert(isStashEmpty());
  Internal::Tree* editableList = Internal::List::PushEmpty();
  for (const Internal::Tree* child : m_list.tree()->children()) {
    PointOfInterest p = pointFromTree(child);
    if (start <= p.abscissa && p.abscissa <= end) {
      Internal::NAry::AddChild(editableList, child->cloneTree());
    }
  }
  m_list = API::JuniorPoolHandle::Builder(editableList);
}

bool PointsOfInterestList::stash(PointOfInterest p) {
  Internal::Tree* newPoint;
  {
    using namespace Internal;
    ExceptionTry {
      if (isStashEmpty()) {
        m_stash = Internal::List::PushEmpty();
      }
      newPoint = Internal::TreeStack::SharedTreeStack->pushPointOfInterest(
          p.abscissa, p.ordinate, p.data, static_cast<uint8_t>(p.interest),
          p.inverted, p.subCurveIndex);
    }
    ExceptionCatch(type) {
      if (type != ExceptionType::TreeStackOverflow) {
        TreeStackCheckpoint::Raise(type);
      }
      return false;
    }
  }
  Internal::NAry::AddChild(m_stash, newPoint);
  return true;
}

void PointsOfInterestList::dropStash() {
  if (!isStashEmpty()) {
    m_stash->removeTree();
    m_stash = nullptr;
  }
}

bool PointsOfInterestList::commit() {
  if (isStashEmpty()) {
    return true;
  }

  /* Move the stash in pool with the main list in a three-steps process, to
   * properly dance around checkpoints:
   *   1. move stash in pool in its own object
   *   2. merge list and stash together in the stack
   *   3. move the merged list back to the pool for safekeeping
   * This order ensures that the pool never contains the same data twice, while
   * also preserving the original list in case of a Raise. */

  API::JuniorPoolHandle pooledStash;
  {
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      pooledStash = API::JuniorPoolHandle::Builder(m_stash);
      assert(!pooledStash.isUninitialized());
      m_stash = nullptr;
    } else {
      dropStash();
      return false;
    }
  }
  assert(!pooledStash.isUninitialized());

  Internal::Tree* stackedList;
  {
    using namespace Internal;
    ExceptionTry {
      stackedList = m_list.tree()->cloneTree();
      Tree* stackedStash = pooledStash.tree()->cloneTree();
      NAry::AddOrMergeChild(stackedList, stackedStash);
    }
    ExceptionCatch(type) {
      if (type != ExceptionType::TreeStackOverflow) {
        TreeStackCheckpoint::Raise(type);
      }
      return false;
    }
  }

  /* No need for a checkpoint, since the combined pool object is smaller than
   * both list and stash separate. */
  m_list = API::JuniorPoolHandle::Builder(stackedList);

  return true;
}

}  // namespace Poincare
