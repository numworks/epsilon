#include <assert.h>
#include <omg/unreachable.h>
#include <omg/utf8_helper.h>
#include <poincare/comparison_operator.h>
#include <string.h>

namespace Poincare {

const char* ComparisonJunior::OperatorString(ComparisonJunior::Operator op) {
  for (const OperatorStringType& opString : k_operatorStrings) {
    if (op == opString.op) {
      return opString.mainString;
    }
  }
  OMG::unreachable();
}

const char* ComparisonJunior::OperatorAlternativeString(
    ComparisonJunior::Operator op) {
  for (const OperatorStringType& opString : k_operatorStrings) {
    if (op == opString.op) {
      return opString.alternativeString;
    }
  }
  OMG::unreachable();
}

ComparisonJunior::Operator ComparisonJunior::OperatorReverseInferiorSuperior(
    ComparisonJunior::Operator op) {
  switch (op) {
    case Operator::Superior:
      return Operator::Inferior;
    case Operator::SuperiorEqual:
      return Operator::InferiorEqual;
    case Operator::Inferior:
      return Operator::Superior;
    default:
      assert(op == Operator::InferiorEqual);
      return Operator::SuperiorEqual;
  }
}

}  // namespace Poincare
