#include "list.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/numeric/statistics_dataset.h>

#include "approximation.h"
#include "k_tree.h"
#include "rational.h"
#include "systematic_reduction.h"
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
      if (Parametric::HasLocalRandom(e)) {
        return nullptr;
      }
      Tree* result = e->child(2)->cloneTree();
      TreeRef value = Integer::Push(k + 1);
      Variables::Replace(result, 0, value);
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
      Tree* result = e->cloneNode();
      if (e->isDepList()) {
        // Do not attempt to reduce conditions of dependency
        reduction = [](Tree*) { return false; };
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

Tree* List::Fold(const Tree* list, TypeBlock type) {
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  // TODO compute ListLength less often
  int size = Dimension::ListLength(list);
  assert(size >= 0);
  if (size == 0) {
    assert(type.isListSum() || type.isListProduct());
    (type.isListSum() ? 0_e : 1_e)->cloneTree();
  }
  for (int i = 0; i < size; i++) {
    Tree* element = GetElement(list, i, SystematicReduction::ShallowReduce);
    assert(element);
    if (i == 0) {
      continue;
    }
    if (type.isListSum() || type.isListProduct()) {
      const Tree* node = type.isListSum() ? KAdd.node<2> : KMult.node<2>;
      result->cloneNodeBeforeNode(node);
      SystematicReduction::ShallowReduce(result);
    } else {
      assert(type.isMin() || type.isMax());
      // Bubble up undefined children.
      if (!result->isUndefined() &&
          (element->isUndefined() ||
           Order::Compare(element, result, Order::OrderType::RealLine) ==
               ((type.isMax()) ? 1 : -1))) {
        result->removeTree();
      } else {
        element->removeTree();
      }
    }
  }
  return result;
}

Tree* List::Variance(const Tree* list, const Tree* coefficients,
                     TypeBlock type) {
  // var(L) = mean(L^2) - mean(L)^2
  KTree variance =
      KAdd(KMean(KPow(KA, 2_e), KB), KMult(-1_e, KPow(KMean(KA, KB), 2_e)));
  // sqrt(var)
  KTree stdDev = KPow(variance, 1_e / 2_e);
  // stdDev * sqrt(1 + 1 / (n - 1))
  KTree sampleStdDev =
      KPow(KMult(stdDev, KAdd(1_e, KPow(KAdd(KC, -1_e), -1_e))), 1_e / 2_e);
  if (type.isSampleStdDev()) {
    Tree* n = coefficients->isOne() ? Integer::Push(Dimension::ListLength(list))
                                    : Fold(coefficients, Type::ListSum);
    PatternMatching::CreateSimplify(sampleStdDev,
                                    {.KA = list, .KB = coefficients, .KC = n});
    n->removeTree();
    return n;
  } else {
    assert(type.isVariance() || type.isStdDev());
    return PatternMatching::CreateSimplify(
        type == Type::Variance ? variance : stdDev,
        {.KA = list, .KB = coefficients});
  }
}

Tree* List::Mean(const Tree* list, const Tree* coefficients) {
  if (coefficients->isOne()) {
    Tree* result = KMult.node<2>->cloneNode();
    Fold(list, Type::ListSum);
    Rational::Push(1, Dimension::ListLength(list));
    SystematicReduction::ShallowReduce(result);
    return result;
  }
  return PatternMatching::CreateSimplify(
      KMult(KListSum(KMult(KA, KB)), KPow(KListSum(KB), -1_e)),
      {.KA = list, .KB = coefficients});
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
      e->moveTreeOverTree(result);
      return true;
    }
    case Type::Mean:
      e->moveTreeOverTree(Mean(e->child(0), e->child(1)));
      return true;
    case Type::Variance:
    case Type::StdDev:
    case Type::SampleStdDev: {
      e->moveTreeOverTree(Variance(e->child(0), e->child(1), e->type()));
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
      Tree* weigthsList = e->child(1);
      if (hasWeightList) {
        // weigths are approximated in place
        BubbleUp(weigthsList, SystematicReduction::ShallowReduce);
        weigthsList->moveTreeOverTree(
            Approximation::RootTreeToTree<T>(weigthsList));
        assert(weigthsList->isList());
      }
      /* values are not approximated in place since we need to keep the exact
       * values to return the exact median */
      Tree* approximatedList = Approximation::RootTreeToTree<T>(valuesList);
      assert(approximatedList->isList());
      TreeDatasetColumn<T> values(approximatedList);
      int upperMedianIndex;
      int lowerMedianIndex;
      if (!hasWeightList) {
        lowerMedianIndex =
            StatisticsDataset<T>(&values).medianIndex(&upperMedianIndex);
      } else {
        TreeDatasetColumn<T> weights(weigthsList);
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
          return changed;
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
