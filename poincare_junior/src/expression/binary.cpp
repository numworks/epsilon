#include "binary.h"

#include <ion/unicode/utf8_helper.h>
#include <omgpj/unicode_helper.h>
#include <poincare_junior/src/memory/pattern_matching.h>

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
      if (  // operatorLength <= maxOperatorLength &&
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
  Tree *subtraction = PatternMatching::CreateAndSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = tree->child(0), .KB = tree->child(1)});
  Sign sign = Sign::Get(subtraction);
  subtraction->removeTree();
  switch (tree->type()) {
    case BlockType::NotEqual:
    case BlockType::Equal:
      if (sign.isZero()) {
        tree->cloneTreeOverTree(tree->isEqual() ? KTrue : KFalse);
        return true;
      }
      if (sign.isStrictlyNegative() || sign.isStrictlyPositive()) {
        tree->cloneTreeOverTree(tree->isEqual() ? KFalse : KTrue);
        return true;
      }
      return false;
    case BlockType::InferiorEqual:
      if (sign.isNegative()) {
        goto return_true;
      }
      if (!sign.canBeNegative() && !sign.canBeNull()) {
        goto return_false;
      }
      return false;
    case BlockType::SuperiorEqual:
      if (sign.isPositive()) {
        goto return_true;
      }
      if (!sign.canBePositive() && !sign.canBeNull()) {
        goto return_false;
      }
      return false;
    case BlockType::Inferior:
      if (sign.isStrictlyNegative()) {
        goto return_true;
      }
      if (!sign.canBeNegative()) {
        goto return_false;
      }
      return false;
    case BlockType::Superior:
      if (sign.isStrictlyPositive()) {
        goto return_true;
      }
      if (!sign.canBePositive()) {
        goto return_false;
      }
      return false;
    default:;
  }
  return false;
return_true:
  tree->cloneTreeOverTree(KTrue);
  return true;
return_false:
  tree->cloneTreeOverTree(KFalse);
  return true;
}

}  // namespace PoincareJ
