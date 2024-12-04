#include "binary.h"

#include <omg/unreachable.h>
#include <omg/utf8_helper.h>
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

ComparisonJunior::Operator Binary::ComparisonOperatorForType(TypeBlock type) {
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
  int maxOperatorLength = name.length;
  int lengthOfFoundOperator = 0;
  Type typeOfFoundOperator;
  for (int i = 0; i < k_numberOfComparisons; i++) {
    Type currentOperatorType = k_operatorForType[i].type;
    const char* currentOperatorString =
        ComparisonJunior::OperatorString(k_operatorForType[i].op);
    // Loop twice, once on the main string, the other on the alternative string
    for (int k = 0; k < 2; k++) {
      int operatorLength =
          UTF8Helper::StringCodePointLength(currentOperatorString);
      if (operatorLength <= maxOperatorLength &&
          operatorLength > lengthOfFoundOperator &&
          CompareLayoutSpanWithNullTerminatedString(
              LayoutSpan(name.start, operatorLength), currentOperatorString) ==
              0) {
        lengthOfFoundOperator = operatorLength;
        typeOfFoundOperator = currentOperatorType;
      }
      currentOperatorString =
          ComparisonJunior::OperatorAlternativeString(k_operatorForType[i].op);
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
      PatternMatching::MatchReplaceSimplify(e, KLogicalXor(KTrue, KA),
                                            KLogicalNot(KA)) ||
      PatternMatching::MatchReplaceSimplify(e, KLogicalXor(KA, KTrue),
                                            KLogicalNot(KA)) ||

      // not (not A) -> A
      PatternMatching::MatchReplace(e, KLogicalNot(KLogicalNot(KA)), KA) ||
      // A or A -> A
      PatternMatching::MatchReplace(e, KLogicalOr(KA, KA), KA) ||
      // A and A -> A
      PatternMatching::MatchReplace(e, KLogicalAnd(KA, KA), KA) ||
      // A xor A -> false
      PatternMatching::MatchReplace(e, KLogicalXor(KA, KA), KFalse);
}

bool Binary::ReduceComparison(Tree* e) {
  assert(e->numberOfChildren() == 2);
  // a < b => a - b < 0 ?
  if (e->isInequality()) {
    ComplexSign signA = GetComplexSign(e->child(0));
    ComplexSign signB = GetComplexSign(e->child(1));
    if (signA.isNonReal() || signB.isNonReal()) {
      e->cloneTreeOverTree(KBadType);
      return true;
    }
    // Do not reduce inequalities if we are not sure to have reals
    if (!(signA.isReal() && signB.isReal())) {
      return false;
    }
  }
  ComplexSign complexSign = ComplexSignOfDifference(e->child(0), e->child(1));
  const Tree* result = nullptr;
  if (!e->isInequality()) {
    // = or !=
    if (complexSign.isNull()) {
      result = e->isEqual() ? KTrue : KFalse;
    } else if (!complexSign.canBeNull()) {
      result = e->isEqual() ? KFalse : KTrue;
    }
  } else {
    assert(complexSign.isReal());
    Sign sign = complexSign.realSign();
    switch (e->type()) {
      case Type::InferiorEqual:
        if (sign.isNegative()) {
          result = KTrue;
        } else if (sign.isStrictlyPositive()) {
          result = KFalse;
        }
        break;
      case Type::SuperiorEqual:
        if (sign.isPositive()) {
          result = KTrue;
        } else if (sign.isStrictlyNegative()) {
          result = KFalse;
        }
        break;
      case Type::Inferior:
        if (sign.isStrictlyNegative()) {
          result = KTrue;
        } else if (sign.isPositive()) {
          result = KFalse;
        }
        break;
      case Type::Superior:
        if (sign.isStrictlyPositive()) {
          result = KTrue;
        } else if (sign.isNegative()) {
          result = KFalse;
        }
        break;
      default:;
    }
  }
  if (!result) {
    return false;
  }
  e->cloneTreeOverTree(result);
  return true;
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

}  // namespace Poincare::Internal
