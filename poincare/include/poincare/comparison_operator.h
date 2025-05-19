#ifndef POINCARE_COMPARISON_OPERATORS_H
#define POINCARE_COMPARISON_OPERATORS_H

#include <assert.h>
#include <omg/troolean.h>
#include <omg/utf8_decoder.h>

#include <array>

namespace Poincare {

// TODO: Rename Comparison when old/comparison.h is deleted
class ComparisonJunior {
 public:
  enum class Operator : uint8_t {
    Equal = 0,
    NotEqual,
    Superior,
    Inferior,
    SuperiorEqual,
    InferiorEqual,
    // Value to know size of enum. Do not use and leave at the end
    NumberOfOperators,
  };

  static const char* OperatorString(Operator op);
  static const char* OperatorAlternativeString(Operator op);

  static Operator OperatorReverseInferiorSuperior(Operator op);

  static bool IsStrict(Operator op) {
    return op == Operator::Superior || op == Operator::Inferior;
  };

 private:
  struct OperatorStringType {
    Operator op;
    const char* mainString;
    const char* alternativeString;
  };

  constexpr static OperatorStringType k_operatorStrings[] = {
      {Operator::Equal, "=", nullptr},
      {Operator::NotEqual, "≠", "!="},  // NFKD norm on "≠"
      {Operator::Superior, ">", nullptr},
      {Operator::Inferior, "<", nullptr},
      {Operator::SuperiorEqual, "≥", ">="},
      {Operator::InferiorEqual, "≤", "<="}};
  static_assert(std::size(k_operatorStrings) ==
                    static_cast<int>(Operator::NumberOfOperators),
                "Missing string for comparison operator.");
};

}  // namespace Poincare

#endif
