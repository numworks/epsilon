#include "list.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/statistics/statistics_dataset.h>

#include "approximation.h"
#include "k_tree.h"
#include "rational.h"
#include "systematic_reduction.h"
#include "undefined.h"
#include "variables.h"

namespace Poincare::Internal {

// TODO: Reduce ListSlice(ListSeq(f(k),k,n),0,m) -> ListSeq(f(k),k,min(n,m))

Tree* List::PushEmpty() { return KList.node<0>->cloneNode(); }

Tree* List::GetElement(const Tree* e, int k, Tree::Operation reduction) {
  switch (e->type()) {
    case Type::List:
      assert(k < e->numberOfChildren());
      return e->child(k)->cloneTree();
    case Type::ListSequence: {
      if (Random::HasRandom(e)) {
        return nullptr;
      }
      Tree* result = e->child(2)->cloneTree();
      TreeRef value = Integer::Push(k + 1);
      Variables::Replace(result, 0, value, true);
      value->removeTree();
      return result;
    }
    case Type::RandIntNoRep:
      return nullptr;  // Should be projected on approximation.
    case Type::ListSort:
    case Type::Median:
      return nullptr;  // If their are still there, their bubble-up failed
    case Type::ListSlice: {
      assert(Integer::Is<uint8_t>(e->child(1)) &&
             Integer::Is<uint8_t>(e->child(2)));
      int startIndex =
          std::max(Integer::Handler(e->child(1)).to<uint8_t>() - 1, 0);
      return GetElement(e->child(0), startIndex + k, reduction);
    }
    default:
      if (e->type().isListToScalar()) {
        return nullptr;
      }
      Tree* result = nullptr;
      if (e->isRandInt()) {
        assert(Random::GetSeed(e) > 0);
        // Cannot clone e directly as it has a non-zero seed
        result = (KRandInt)->cloneNode();
        Random::SetSeed(result, Random::GetSeed(e) + k);
      } else {
        result = e->cloneNode();
        if (e->isDepList()) {
          // Do not attempt to reduce conditions of dependency
          reduction = [](Tree*) { return false; };
        }
      }
      for (const Tree* child : e->children()) {
        if (!GetElement(child, k, reduction)) {
          SharedTreeStack->dropBlocksFrom(result);
          return nullptr;
        }
      }
      reduction(result);
      return result;
  }
}

static Tree* FoldSumOrProduct(const Tree* list, TypeBlock type) {
  assert(type.isListSum() || type.isListProduct());
  // TODO compute ListLength less often
  int size = Dimension::ListLength(list);
  assert(size >= 0);
  if (size == 0) {
    return (type.isListSum() ? 0_e : 1_e)->cloneTree();
  }
  Tree* result = (type.isListSum() ? SharedTreeStack->pushAdd(0)
                                   : SharedTreeStack->pushMult(0));
  for (int i = 0; i < size; i++) {
    Tree* element =
        List::GetElement(list, i, SystematicReduction::ShallowReduce);
    if (element == nullptr) {
      // If an element is not retrievable, abort and return [nullptr]
      SharedTreeStack->flushFromBlock(result);
      return nullptr;
    }
  }
  NAry::SetNumberOfChildren(result, size);
  SystematicReduction::ShallowReduce(result);
  return result;
}

static Tree* FoldMinOrMax(const Tree* list, TypeBlock type) {
  assert(type.isMin() || type.isMax());
  // TODO compute ListLength less often
  int size = Dimension::ListLength(list);
  assert(size > 0);
  Tree* result = List::GetElement(list, 0, SystematicReduction::ShallowReduce);
  if (result == nullptr) {
    return nullptr;
  }
  for (int i = 1; i < size; i++) {
    Tree* element =
        List::GetElement(list, i, SystematicReduction::ShallowReduce);
    if (element == nullptr) {
      // If an element is not retrievable, abort and return [nullptr]
      SharedTreeStack->flushFromBlock(result);
      return nullptr;
    }
    // Bubble up undefined children.
    if (element->isUndefined()) {
      result->removeTree();
      return result;
    }
    (Order::Compare(element, result, Order::OrderType::RealLine) ==
             ((type.isMax()) ? 1 : -1)
         ? result
         : element)
        ->removeTree();
  }
  return result;
}

/* Return the folded tree on success, return [nullptr] on failure */
Tree* List::Fold(const Tree* list, TypeBlock type) {
  if (type.isListSum() || type.isListProduct()) {
    return FoldSumOrProduct(list, type);
  }
  return FoldMinOrMax(list, type);
}

Tree* List::Variance(const Tree* list, const Tree* coefficients,
                     TypeBlock type) {
#if POINCARE_LIST
  // var(L) = mean(L^2) - mean(L)^2
  SimpleKTrees::KTree variance =
      KAdd(KMean(KPow(KA, 2_e), KB), KMult(-1_e, KPow(KMean(KA, KB), 2_e)));
  // sqrt(var)
  SimpleKTrees::KTree stdDev = KPow(variance, 1_e / 2_e);
  // stdDev * sqrt(1 + 1 / (n - 1))
  SimpleKTrees::KTree sampleStdDev =
      KMult(stdDev, KPow(KAdd(1_e, KPow(KAdd(KC, -1_e), -1_e)), 1_e / 2_e));
  PatternMatching::Context ctx({.KA = list, .KB = coefficients});
  ctx.setInvolvesList(true);
  if (type.isSampleStdDev()) {
    Tree* n = coefficients->isOne()
                  ? Integer::Push(Dimension::ListLength(list))
                  : FoldSumOrProduct(coefficients, Type::ListSum);
    if (n == nullptr) {
      return nullptr;
    }
    ctx.setNode(KC, n, 1, false);
    PatternMatching::CreateSimplify(sampleStdDev, ctx);
    n->removeTree();
    return n;
  } else {
    assert(type.isVariance() || type.isStdDev());
    return PatternMatching::CreateSimplify(
        type == Type::Variance ? variance : stdDev, ctx);
  }
#else
  OMG::unreachable();
#endif
}

Tree* List::Mean(const Tree* list, const Tree* coefficients) {
#if POINCARE_LIST
  if (coefficients->isOne()) {
    Tree* result = KMult.node<2>->cloneNode();
    Tree* listSum = FoldSumOrProduct(list, Type::ListSum);
    if (listSum == nullptr) {
      SharedTreeStack->flushFromBlock(result);
      return nullptr;
    }
    Rational::Push(1, Dimension::ListLength(list));
    SystematicReduction::ShallowReduce(result);
    return result;
  }
  assert(Dimension::IsList(coefficients));
  PatternMatching::Context ctx({.KA = list, .KB = coefficients});
  ctx.setInvolvesList(true);
  return PatternMatching::CreateSimplify(
      KMult(KListSum(KMult(KA, KB)), KPow(KListSum(KB), -1_e)), ctx);
#else
  OMG::unreachable();
#endif
}

bool List::BubbleUp(Tree* e, Tree::Operation reduction) {
  int length = Dimension::ListLength(e);
  if (length < 0 || e->isList()) {
    return false;
  }
  Tree* list = List::PushEmpty();
  for (int i = 0; i < length; i++) {
    Tree* element = GetElement(e, i, reduction);
    if (!element) {
      assert(i == 0);
      list->removeTree();
      return false;
    }
  }
  NAry::SetNumberOfChildren(list, length);
  e->moveTreeOverTree(list);
  return true;
}

bool List::ShallowApplyListOperators(Tree* e) {
  switch (e->type()) {
    case Type::ListSum:
    case Type::ListProduct:
    case Type::Min:
    case Type::Max: {
      Tree* result;
      ExceptionTry { result = Fold(e->child(0), e->type()); }
      ExceptionCatch(exc) {
        if (exc == ExceptionType::SortFail) {
          if (Variables::HasVariables(e) || Variables::HasUserSymbols(e)) {
            // leave max({x,0}) unchanged
            // TODO: we could return undef with max({x,i})
            return false;
          }
          e->cloneTreeOverTree(KUndef);
          return true;
        }
        TreeStackCheckpoint::Raise(exc);
      }
      if (result) {
        e->moveTreeOverTree(result);
        return true;
      }
      return false;
    }
    case Type::Mean: {
      Tree* mean = Mean(e->child(0), e->child(1));
      if (mean == nullptr) {
        return false;
      }
      e->moveTreeOverTree(mean);
      return true;
    }
    case Type::Variance:
    case Type::StdDev:
    case Type::SampleStdDev: {
      Tree* variance = Variance(e->child(0), e->child(1), e->type());
      if (variance == nullptr) {
        return false;
      }
      e->moveTreeOverTree(variance);
      return true;
    }
    case Type::Median: {
      // precision used for comparisons
      using T = double;
      bool hasWeightList = Dimension::IsList(e->child(1));
      Tree* valuesList = e->child(0);
      BubbleUp(valuesList, SystematicReduction::ShallowReduce);
      if (!valuesList->isList()) {
        return false;
      }
      Tree* weightsList = e->child(1);
      if (hasWeightList) {
        // weights are approximated in place
        BubbleUp(weightsList, SystematicReduction::ShallowReduce);
        weightsList->moveTreeOverTree(
            Approximation::ToTree<T>(weightsList, Approximation::Parameters{}));
        assert(weightsList->isList());
      }
      /* values are not approximated in place since we need to keep the exact
       * values to return the exact median */
      Tree* approximatedList =
          Approximation::ToTree<T>(valuesList, Approximation::Parameters{});
      assert(approximatedList->isList());
      TreeDatasetColumn<T> values(approximatedList);
      int upperMedianIndex;
      int lowerMedianIndex;
      if (!hasWeightList) {
        lowerMedianIndex =
            StatisticsDataset<T>(&values).medianIndex(&upperMedianIndex);
      } else {
        TreeDatasetColumn<T> weights(weightsList);
        lowerMedianIndex = StatisticsDataset<T>(&values, &weights)
                               .medianIndex(&upperMedianIndex);
      }
      approximatedList->removeTree();
      if (lowerMedianIndex < 0) {
        e->cloneTreeOverTree(KUndef);
      } else if (upperMedianIndex == lowerMedianIndex) {
        e->moveTreeOverTree(valuesList->child(lowerMedianIndex));
      } else {
        e->moveTreeOverTree(PatternMatching::CreateSimplify(
            KMult(1_e / 2_e, KAdd(KA, KB)),
            {.KA = valuesList->child(lowerMedianIndex),
             .KB = valuesList->child(upperMedianIndex)}));
      }
      return true;
    }
    case Type::ListSort: {
      Tree* list = e->child(0);
      bool changed = BubbleUp(list, SystematicReduction::ShallowReduce);
      if (!list->isList()) {
        return changed;
      }
      ExceptionTry { NAry::Sort(list, Order::OrderType::RealLine); }
      ExceptionCatch(exc) {
        if (exc == ExceptionType::SortFail) {
          if (Variables::HasVariables(e) || Variables::HasUserSymbols(e)) {
            return changed;
          }
          /* Returning undef instead of the list changes the list length of the
           * expression. It might work when used with other lists because a
           * scalar can be interpret as a constant list in most contexts but
           * expect issues. */
          Undefined::ReplaceTreeWithDimensionedType(e,
                                                    Type::UndefOutOfDefinition);
          return true;
        }
        TreeStackCheckpoint::Raise(exc);
      }
      e->removeNode();
      return true;
    }
    case Type::ListElement: {
      assert(Integer::Is<uint8_t>(e->child(1)));
      int i = Integer::Handler(e->child(1)).to<uint8_t>() - 1;
      if (i < 0 || i >= e->child(0)->numberOfChildren()) {
        e->cloneTreeOverTree(KUndef);
        return true;
      }
      Tree* element =
          GetElement(e->child(0), i, SystematicReduction::ShallowReduce);
      if (!element) {
        return false;
      }
      e->moveTreeOverTree(element);
      return true;
    }
    case Type::ListSlice: {
      int minIndex = 1;
      int maxIndex = Dimension::ListLength(e->child(0));
      TreeRef startIndex = e->child(1);
      TreeRef endIndex = e->child(2);
      assert(Integer::Is<uint8_t>(startIndex) &&
             Integer::Is<uint8_t>(endIndex));
      bool changed = false;
      if (Integer::Handler(startIndex).to<uint8_t>() < minIndex) {
        startIndex->moveTreeOverTree(Integer::Push(minIndex));
        changed = true;
      }
      if (Integer::Handler(endIndex).to<uint8_t>() > maxIndex) {
        endIndex->moveTreeOverTree(Integer::Push(maxIndex));
        changed = true;
      }
      return changed;
    }
    case Type::Dim:
      e->moveTreeOverTree(Integer::Push(Dimension::ListLength(e->child(0))));
      return true;
    default:
      return false;
  }
}

}  // namespace Poincare::Internal
