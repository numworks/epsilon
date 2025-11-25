#include "binary.h"

#include <omg/unreachable.h>
#include <omg/utf8_helper.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "k_tree.h"
#include "sign.h"

namespace Poincare::Internal {

bool Binary::IsBinaryLogicalOperator(LayoutSpan name, Type* type) {
  for (int i = 0; i < k_numberOfOperators; i++) {
    if (CompareLayoutSpanWithNullTerminatedString(
            name, k_operatorNames[i].name) == 0) {
      if (type) {
        *type = k_operatorNames[i].type;
      }
      return true;
    }
  }
  return false;
}

const char* Binary::OperatorName(TypeBlock type) {
  if (type.isLogicalNot()) {
    return k_logicalNotName;
  }
  for (const TypeAndName& name : k_operatorNames) {
    if (type == name.type) {
      return name.name;
    }
  }
  OMG::unreachable();
}

Comparison::Operator Binary::ComparisonOperatorForType(TypeBlock type) {
  assert(type.isComparison());
  for (const OperatorForType& opForType : k_operatorForType) {
    if (type == opForType.type) {
      return opForType.op;
    }
  }
  OMG::unreachable();
}

bool Binary::IsComparisonOperatorString(LayoutSpan name, Type* returnType,
                                        size_t* returnLength) {
  size_t maxOperatorLength = name.size();
  size_t lengthOfFoundOperator = 0;
  Type typeOfFoundOperator;
  for (int i = 0; i < k_numberOfComparisons; i++) {
    Type currentOperatorType = k_operatorForType[i].type;
    const char* currentOperatorString =
        Comparison::OperatorString(k_operatorForType[i].op);
    // Loop twice, once on the main string, the other on the alternative string
    for (int k = 0; k < 2; k++) {
      size_t operatorLength =
          UTF8Helper::StringGlyphLength(currentOperatorString);
      if (operatorLength <= maxOperatorLength &&
          operatorLength > lengthOfFoundOperator &&
          CompareLayoutSpanWithNullTerminatedString(
              LayoutSpan(name.begin(), operatorLength),
              currentOperatorString) == 0) {
        lengthOfFoundOperator = operatorLength;
        typeOfFoundOperator = currentOperatorType;
      }
      currentOperatorString =
          Comparison::OperatorAlternativeString(k_operatorForType[i].op);
      if (!currentOperatorString) {
        break;
      }
    }
  }
  if (lengthOfFoundOperator == 0) {
    return false;
  }
  *returnLength = lengthOfFoundOperator;
  *returnType = typeOfFoundOperator;
  return true;
}

/* TODO:
 * - advanced simplifications such as:
 *   not A and A => false
 *   A or not A => true
 *   not (A and B) => not A or not B
 *   not (A or B) => not A and not B
 *   A xor B => (not A and B) or (A and not B)
 *   distribute or on and etc
 *
 * - logical operators should be n-ary
 *
 * - introduce boolean unknowns
 *
 * - hook a SAT solver
 */

bool Binary::ReduceBooleanOperator(Tree* e) {
#if POINCARE_BOOLEAN
  return
      // not true -> false
      PatternMatching::MatchReplace(e, KLogicalNot(KTrue), KFalse) ||
      // not false -> true
      PatternMatching::MatchReplace(e, KLogicalNot(KFalse), KTrue) ||
      // false and A -> false
      PatternMatching::MatchReplace(e, KLogicalAnd(KFalse, KA), KFalse) ||
      PatternMatching::MatchReplace(e, KLogicalAnd(KA, KFalse), KFalse) ||
      // true and A -> A
      PatternMatching::MatchReplace(e, KLogicalAnd(KTrue, KA), KA) ||
      PatternMatching::MatchReplace(e, KLogicalAnd(KA, KTrue), KA) ||
      // true or A -> true
      PatternMatching::MatchReplace(e, KLogicalOr(KTrue, KA), KTrue) ||
      PatternMatching::MatchReplace(e, KLogicalOr(KA, KTrue), KTrue) ||
      // false or A -> A
      PatternMatching::MatchReplace(e, KLogicalOr(KFalse, KA), KA) ||
      PatternMatching::MatchReplace(e, KLogicalOr(KA, KFalse), KA) ||
      // false xor A -> A
      PatternMatching::MatchReplace(e, KLogicalXor(KFalse, KA), KA) ||
      PatternMatching::MatchReplace(e, KLogicalXor(KA, KFalse), KA) ||
      // true xor A -> not A
      PatternMatching::MatchReplaceReduce(e, KLogicalXor(KTrue, KA),
                                          KLogicalNot(KA)) ||
      PatternMatching::MatchReplaceReduce(e, KLogicalXor(KA, KTrue),
                                          KLogicalNot(KA)) ||

      // not (not A) -> A
      PatternMatching::MatchReplace(e, KLogicalNot(KLogicalNot(KA)), KA) ||
      // A or A -> A
      PatternMatching::MatchReplace(e, KLogicalOr(KA, KA), KA) ||
      // A and A -> A
      PatternMatching::MatchReplace(e, KLogicalAnd(KA, KA), KA) ||
      // A xor A -> false
      PatternMatching::MatchReplace(e, KLogicalXor(KA, KA), KFalse);
#else
  return false;
#endif
}

bool Binary::ReduceComparison(Tree* e) {
#if POINCARE_BOOLEAN
  assert(e->numberOfChildren() == 2);
  // a < b => a - b < 0 ?
  if (e->isInequality()) {
    ComplexProperties propertiesA = GetComplexProperties(e->child(0));
    ComplexProperties propertiesB = GetComplexProperties(e->child(1));
    if (propertiesA.isNonReal() || propertiesB.isNonReal()) {
      e->cloneTreeOverTree(KUndefBoolean);
      return true;
    }
    // Do not reduce inequalities if we are not sure to have reals
    if (!(propertiesA.isReal() && propertiesB.isReal())) {
      return false;
    }
  }
  ComplexProperties complexProperties =
      ComplexPropertiesOfDifference(e->child(0), e->child(1));
  const Tree* result = nullptr;
  if (!e->isInequality()) {
    // = or !=
    if (complexProperties.isNull()) {
      result = e->isEqual() ? KTrue : KFalse;
    } else if (!complexProperties.canBeNull()) {
      result = e->isEqual() ? KFalse : KTrue;
    }
  } else {
    assert(complexProperties.isReal());
    Sign sign = complexProperties.realSign();
    OMG::Troolean isPositive = sign.trooleanIsPositive();
    OMG::Troolean isStrictlyPositive = sign.trooleanIsStrictlyPositive();
    switch (e->type()) {
      case Type::InferiorEqual:
        result = isStrictlyPositive == OMG::Troolean::True    ? KFalse
                 : isStrictlyPositive == OMG::Troolean::False ? KTrue
                                                              : result;
        break;
      case Type::SuperiorEqual:
        result = isPositive == OMG::Troolean::True    ? KTrue
                 : isPositive == OMG::Troolean::False ? KFalse
                                                      : result;
        break;
      case Type::Inferior:
        result = isPositive == OMG::Troolean::True    ? KFalse
                 : isPositive == OMG::Troolean::False ? KTrue
                                                      : result;
        break;
      case Type::Superior:
        result = isStrictlyPositive == OMG::Troolean::True    ? KTrue
                 : isStrictlyPositive == OMG::Troolean::False ? KFalse
                                                              : result;
        break;
      default:;
    }
  }
  if (!result) {
    return false;
  }
  e->cloneTreeOverTree(result);
  return true;
#else
  e->cloneTreeOverTree(KUndef);
  return true;
#endif
}

bool Binary::ReducePiecewise(Tree* piecewise) {
  int n = piecewise->numberOfChildren();
  int i = 0;
  Tree* child = piecewise->child(0);
  while (i + 1 < n) {
    Tree* condition = child->nextTree();
    if (condition->isFalse()) {
      // Remove this clause
      child->removeTree();
      child->removeTree();
      n -= 2;
      continue;
    }
    if (condition->isTrue()) {
      // Drop remaining clauses
      for (int j = i + 1; j < n; j++) {
        condition->removeTree();
      }
      n = i + 1;
      break;
    }
    /* TODO: We could simplify conditions under the assumption that the previous
     * conditions are false. */
    i += 2;
    child = condition->nextTree();
  }
  bool changed = n != piecewise->numberOfChildren();
  if (changed) {
    NAry::SetNumberOfChildren(piecewise, n);
  }
  if (n == 1) {
    piecewise->removeNode();
    return true;
  }
  if (n == 0) {
    piecewise->cloneTreeOverTree(KUndef);
  }
  return changed;
}

#if POINCARE_PIECEWISE

bool Binary::MakeLenient(Tree* e) {
  // Booleans should have been reduced out.
  assert(!e->isBoolean());
  if (e->isComparison()) {
    switch (e->type()) {
      case Type::Inferior:
        e->cloneNodeOverNode(KInferiorEqual);
        break;
      case Type::Superior:
        e->cloneNodeOverNode(KSuperiorEqual);
        break;
      case Type::NotEqual:
        e->cloneTreeOverTree(KTrue);
        return true;
      case Type::InferiorEqual:
      case Type::SuperiorEqual:
      case Type::Equal:
        return true;
      default:
        OMG::unreachable();
    }
    assert(!ReduceComparison(e));
    return true;
  }
  if (!e->isLogicalOperator()) {
    return false;
  }
  bool result = false;
  switch (e->type()) {
    case Type::LogicalNot:
      result = MakeStrict(e->child(0));
      break;
    case Type::LogicalAnd:
    case Type::LogicalOr:
      result = MakeLenient(e->child(0)) && MakeLenient(e->child(1));
      break;
    case Type::LogicalXor:
      // TODO: Not handled for now. It should be expanded into a!b+!ab first
      result = false;
      break;
    case Type::LogicalNor:
    case Type::LogicalNand:
      // Nor and Nand should have been projected.
      // result = MakeStrict(e->child(0)) && MakeStrict(e->child(1));
    default:
      OMG::unreachable();
  }
  if (result) {
    ReduceBooleanOperator(e);
  }
  return result;
}

bool Binary::MakeStrict(Tree* e) {
  // Booleans should have been reduced out.
  assert(!e->isBoolean());
  if (e->isComparison()) {
    switch (e->type()) {
      case Type::Inferior:
      case Type::Superior:
      case Type::NotEqual:
        return true;
      case Type::InferiorEqual:
        e->cloneNodeOverNode(KInferior);
        break;
      case Type::SuperiorEqual:
        e->cloneNodeOverNode(KSuperior);
        break;
      case Type::Equal:
        e->cloneTreeOverTree(KFalse);
        return true;

      default:
        OMG::unreachable();
    }
    assert(!ReduceComparison(e));
    return true;
  }
  if (!e->isLogicalOperator()) {
    return false;
  }
  bool result = false;
  switch (e->type()) {
    case Type::LogicalNot:
      result = MakeLenient(e->child(0));
      break;
    case Type::LogicalAnd:
    case Type::LogicalOr:
      result = MakeStrict(e->child(0)) && MakeStrict(e->child(1));
      break;
    case Type::LogicalXor:
      // TODO: Not handled for now. It should be expanded into a!b+!ab first
      result = false;
      break;
    case Type::LogicalNor:
    case Type::LogicalNand:
      // Nor and Nand should have been projected.
      // result = MakeLenient(e->child(0)) && MakeLenient(e->child(1));
    default:
      OMG::unreachable();
  }
  if (result) {
    ReduceBooleanOperator(e);
  }
  return result;
}  // namespace Poincare::Internal

Tree* Binary::ReducePiecewiseDerivative(const Tree* symbol,
                                        const Tree* symbolValue,
                                        const Tree* order,
                                        const Tree* derivand) {
  assert(derivand->isPiecewise());
  int numberOfChildren = derivand->numberOfChildren();
  Tree* result = derivand->cloneNode();
  for (IndexedChild<const Tree*> derivandChild : derivand->indexedChildren()) {
    if (derivandChild.index % 2 == 0) {
      // Derive branch
      PatternMatching::CreateReduce(
          KDiff(KA, KB, KC, KD),
          {.KA = symbol, .KB = symbolValue, .KC = order, .KD = derivandChild},
          {.KD = 1});
    } else {
      // Clone strict condition and leave scope.
      Tree* strictCondition = derivandChild->cloneTree();
      if (!Binary::MakeStrict(strictCondition)) {
        SharedTreeStack->flushFromBlock(result);
        return nullptr;
      }
      Variables::LeaveScopeWithReplacement(strictCondition, symbolValue, true,
                                           false);
      numberOfChildren += 2;
      KUndef->cloneTree();
      // Clone nonStrict condition and leave scope.
      strictCondition = derivandChild->cloneTree();
      if (!Binary::MakeLenient(strictCondition)) {
        SharedTreeStack->flushFromBlock(result);
        return nullptr;
      }
      Variables::LeaveScopeWithReplacement(strictCondition, symbolValue, true,
                                           false);
    }
  }
  NAry::SetNumberOfChildren(result, numberOfChildren);
  SystematicReduction::ShallowReduce(result);
  return result;
}
#endif

}  // namespace Poincare::Internal
