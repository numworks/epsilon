#include "binary.h"

#include <ion/unicode/utf8_helper.h>
#include <omgpj/unicode_helper.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"

namespace PoincareJ {

bool Binary::IsBinaryLogicalOperator(const CPL *name, int nameLength,
                                     BlockType *type) {
  for (int i = 0; i < k_numberOfOperators; i++) {
    if (OMG::CompareCPLWithNullTerminatedString(name, nameLength,
                                                k_operatorNames[i].name) == 0) {
      if (type) {
        *type = k_operatorNames[i].type;
      }
      return true;
    }
  }
  return false;
}

const char *Binary::OperatorName(TypeBlock type) {
  if (type.isLogicalNot()) {
    return k_logicalNotName;
  }
  for (const TypeAndName &name : k_operatorNames) {
    if (type == name.type) {
      return name.name;
    }
  }
  assert(false);
}

const char *Binary::ComparisonOperatorName(TypeBlock type) {
  for (const OperatorString &name : k_operatorStrings) {
    if (type == name.type) {
      return name.mainString;
    }
  }
  assert(false);
}

bool Binary::IsComparisonOperatorString(const CPL *s, int length,
                                        BlockType *returnType,
                                        size_t *returnLength) {
  int maxOperatorLength = length;
  int lengthOfFoundOperator = 0;
  BlockType typeOfFoundOperator;
  for (int i = 0; i < k_numberOfComparisons; i++) {
    const char *currentOperatorString = k_operatorStrings[i].mainString;
    // Loop twice, once on the main string, the other on the alternative string
    for (int k = 0; k < 2; k++) {
      int operatorLength = UTF8Helper::StringGlyphLength(currentOperatorString);
      if (operatorLength <= maxOperatorLength &&
          operatorLength > lengthOfFoundOperator &&
          OMG::CompareCPLWithNullTerminatedString(s, operatorLength,

                                                  currentOperatorString) == 0) {
        lengthOfFoundOperator = operatorLength;
        typeOfFoundOperator = k_operatorStrings[i].type;
      }
      currentOperatorString = k_operatorStrings[i].alternativeString;
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

bool Binary::SimplifyBooleanOperator(Tree *tree) {
  return
      // not true -> false
      PatternMatching::MatchAndReplace(tree, KLogicalNot(KTrue), KFalse) ||
      // not false -> true
      PatternMatching::MatchAndReplace(tree, KLogicalNot(KFalse), KTrue) ||
      // false and A -> false
      PatternMatching::MatchAndReplace(tree, KLogicalAnd(KFalse, KA), KFalse) ||
      PatternMatching::MatchAndReplace(tree, KLogicalAnd(KA, KFalse), KFalse) ||
      // true and A -> A
      PatternMatching::MatchAndReplace(tree, KLogicalAnd(KTrue, KA), KA) ||
      PatternMatching::MatchAndReplace(tree, KLogicalAnd(KA, KTrue), KA) ||
      // true or A -> true
      PatternMatching::MatchAndReplace(tree, KLogicalOr(KTrue, KA), KTrue) ||
      PatternMatching::MatchAndReplace(tree, KLogicalOr(KA, KTrue), KTrue) ||
      // false or A -> A
      PatternMatching::MatchAndReplace(tree, KLogicalOr(KFalse, KA), KA) ||
      PatternMatching::MatchAndReplace(tree, KLogicalOr(KA, KFalse), KA) ||
      // false xor A -> A
      PatternMatching::MatchAndReplace(tree, KLogicalXor(KFalse, KA), KA) ||
      PatternMatching::MatchAndReplace(tree, KLogicalXor(KA, KFalse), KA) ||
      // true xor A -> not A
      PatternMatching::MatchReplaceAndSimplify(tree, KLogicalXor(KTrue, KA),
                                               KLogicalNot(KA)) ||
      PatternMatching::MatchReplaceAndSimplify(tree, KLogicalXor(KA, KTrue),
                                               KLogicalNot(KA)) ||

      // not (not A) -> A
      PatternMatching::MatchAndReplace(tree, KLogicalNot(KLogicalNot(KA)),
                                       KA) ||
      // A or A -> A
      PatternMatching::MatchAndReplace(tree, KLogicalOr(KA, KA), KA) ||
      // A and A -> A
      PatternMatching::MatchAndReplace(tree, KLogicalAnd(KA, KA), KA) ||
      // A xor A -> false
      PatternMatching::MatchAndReplace(tree, KLogicalXor(KA, KA), KFalse);
}

bool Binary::SimplifyComparison(Tree *tree) {
  assert(tree->numberOfChildren() == 2);
  // a < b => a - b < 0 ?
  if (tree->isInequality()) {
    ComplexSign signA = ComplexSign::Get(tree->child(0));
    ComplexSign signB = ComplexSign::Get(tree->child(1));
    if (signA.isComplex() || signB.isComplex()) {
      tree->cloneTreeOverTree(KUndef);
      return true;
    }
    // Do not reduce inequalities if we are not sure to have reals
    if (!(signA.isReal() && signB.isReal())) {
      return false;
    }
  }
  ComplexSign complexSign =
      ComplexSign::SignOfDifference(tree->child(0), tree->child(1));
  const Tree *result = nullptr;
  if (!tree->isInequality()) {
    // = or !=
    if (complexSign.isZero()) {
      result = tree->isEqual() ? KTrue : KFalse;
    } else if (!complexSign.canBeNull()) {
      result = tree->isEqual() ? KFalse : KTrue;
    }
  } else {
    assert(complexSign.isReal());
    Sign sign = complexSign.realSign();
    switch (tree->type()) {
      case BlockType::InferiorEqual:
        if (sign.isNegative()) {
          result = KTrue;
        } else if (sign.isStrictlyPositive()) {
          result = KFalse;
        }
        break;
      case BlockType::SuperiorEqual:
        if (sign.isPositive()) {
          result = KTrue;
        } else if (sign.isStrictlyNegative()) {
          result = KFalse;
        }
        break;
      case BlockType::Inferior:
        if (sign.isStrictlyNegative()) {
          result = KTrue;
        } else if (sign.isPositive()) {
          result = KFalse;
        }
        break;
      case BlockType::Superior:
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
  tree->cloneTreeOverTree(result);
  return true;
}

bool Binary::SimplifyPiecewise(Tree *piecewise) {
  int n = piecewise->numberOfChildren();
  int i = 0;
  Tree *child = piecewise->child(0);
  while (i + 1 < n) {
    Tree *condition = child->nextTree();
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

}  // namespace PoincareJ
