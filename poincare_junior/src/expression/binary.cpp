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

/* TODO advanced simplifications such as:
 *   not A and A => false
 *   A or not A => true
 *   not (A and B) => not A or not B
 *   not (A or B) => not A and not B
 *   A xor B => (not A and B) or (A and not B)
 *   distribute or on and etc
 */

}  // namespace PoincareJ
