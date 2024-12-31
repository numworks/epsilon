#include <omg/list.h>
#include <poincare/k_tree.h>
#include <poincare/numeric/point_of_interest_list.h>
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
  const Internal::Tree* result =
      t->child(0) + i * Internal::TypeBlock::NumberOfMetaBlocks(
                            Internal::Type::PointOfInterest);
  assert(result == t->child(i));
  return result;
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

void PointsOfInterestList::init() {
  m_list = API::JuniorPoolHandle::Builder(KList());
}

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
  return pointFromTree(pointAddressInTree(m_list.tree(), i));
}

void PointsOfInterestList::sort() {
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
  Internal::Tree* editableList = Internal::List::PushEmpty();
  for (const Internal::Tree* child : m_list.tree()->children()) {
    PointOfInterest p = pointFromTree(child);
    if (start <= p.abscissa && p.abscissa <= end) {
      Internal::NAry::AddChild(editableList, child->cloneTree());
    }
  }
  m_list = API::JuniorPoolHandle::Builder(editableList);
}

API::JuniorPoolHandle PointsOfInterestList::BuildStash(Provider provider,
                                                       void* providerContext) {
  {
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Internal::Tree* stash;
      {
        using namespace Internal;
        ExceptionTry {
          stash = List::PushEmpty();
          PointOfInterest point;
          while (!(point = provider(providerContext)).isUninitialized()) {
            Tree* pointTree =
                Internal::TreeStack::SharedTreeStack->pushPointOfInterest(
                    point.abscissa, point.ordinate, point.data,
                    static_cast<uint8_t>(point.interest), point.inverted,
                    point.subCurveIndex);
            NAry::AddChild(stash, pointTree);
          }
        }
        ExceptionCatch(type) {
          if (type != ExceptionType::TreeStackOverflow) {
            TreeStackCheckpoint::Raise(type);
          }
          return {};
        }
      }

      return API::JuniorPoolHandle::Builder(stash);
    } else {
      return {};
    }
  }
}

bool PointsOfInterestList::merge(API::JuniorPoolHandle& stash) {
  assert(!stash.isUninitialized());
  // Merge list and stash together in the stack.
  Internal::Tree* stackedList;
  {
    using namespace Internal;
    ExceptionTry {
      stackedList = m_list.tree()->cloneTree();
      Tree* stackedStash = stash.tree()->cloneTree();
      NAry::AddOrMergeChild(stackedList, stackedStash);
    }
    ExceptionCatch(type) {
      if (type != ExceptionType::TreeStackOverflow) {
        TreeStackCheckpoint::Raise(type);
      }
      return false;
    }
  }
  assert(stackedList->isList() &&
         !stackedList->hasChildSatisfying(
             [](const Internal::Tree* t) { return !t->isPointOfInterest(); }));
  /* No need for a checkpoint, since the combined pool object is smaller
   * than both list and stash separate. */
  m_list = API::JuniorPoolHandle::Builder(stackedList);
  return true;
}

}  // namespace Poincare
