#include <apps/shared/global_context.h>
#include <poincare/expression.h>
#include <poincare/expression_or_float.h>
#include <poincare/helpers/layout.h>
#include <poincare/k_tree.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/derivation.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/layout/layout_serializer.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_expression_serializes_to(
    Poincare::UserExpression expression, const char* serialization,
    bool compactMode = false,
    Poincare::Preferences::PrintFloatMode printFloatMode =
        Poincare::Preferences::PrintFloatMode::Decimal,
    int numberOfSignificantDigits =
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits) {
  constexpr int bufferSize = 100;
  char buffer[bufferSize];
  expression.serialize(buffer, compactMode, printFloatMode,
                       numberOfSignificantDigits);
  quiz_assert_print_if_failure((strcmp(serialization, buffer) == 0),
                               serialization, serialization, buffer);
}

void assert_expression_or_float_serializes_to(
    Poincare::ExpressionOrFloat expressionOrFloat, const char* serialization,
    Poincare::Preferences::PrintFloatMode printFloatMode =
        Poincare::Preferences::PrintFloatMode::Decimal,
    int numberOfSignificantDigits =
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits,
    size_t maxGlyphLength = Poincare::PrintFloat::k_maxFloatGlyphLength) {
  constexpr int bufferSize = 100;
  char buffer[bufferSize];
  expressionOrFloat.writeText(
      buffer,
      Poincare::ExpressionOrFloat::ApproximationParameters{
          Poincare::Preferences::AngleUnit::Radian,
          Poincare::Preferences::ComplexFormat::Real},
      numberOfSignificantDigits, printFloatMode, maxGlyphLength);
  quiz_assert_print_if_failure((strcmp(serialization, buffer) == 0),
                               serialization, serialization, buffer);
}

void assert_beautifies_serializes_to(
    const Tree* rational, const char* serialization,
    Poincare::Preferences::PrintFloatMode mode = ScientificMode,
    int numberOfSignificantDigits = 7, OMG::Base base = OMG::Base::Decimal) {
  Tree* clone = rational->cloneTree();
  // Serialization is supposed to be performed on beautified expressions
  Beautification::DeepBeautify(clone);
  assert_expression_serializes_to(clone, serialization, mode,
                                  numberOfSignificantDigits, base);
  clone->removeTree();
}

template <typename T>
void assert_float_serializes_to(
    T value, const char* result,
    Poincare::Preferences::PrintFloatMode mode = ScientificMode,
    int numberOfSignificantDigits = 7) {
  Tree* e = SharedTreeStack->pushFloat(value);
  assert_expression_serializes_to(e, result, mode, numberOfSignificantDigits);
  e->removeTree();
}

QUIZ_CASE(pcj_serialization_float) {
  assert_float_serializes_to<double>(-1.23456789E30, "-1.23456789ᴇ30",
                                     DecimalMode, 14);
  assert_float_serializes_to<double>(1.23456789E30, "1.23456789ᴇ30",
                                     DecimalMode, 14);
  assert_float_serializes_to<double>(-1.23456789E-30, "-1.23456789ᴇ-30",
                                     DecimalMode, 14);
  assert_float_serializes_to<double>(-1.2345E-3, "-0.0012345", DecimalMode);
  assert_float_serializes_to<double>(1.2345E-3, "0.0012345", DecimalMode);
  assert_float_serializes_to<double>(1.2345E3, "1234.5", DecimalMode);
  assert_float_serializes_to<double>(-1.2345E3, "-1234.5", DecimalMode);
  assert_float_serializes_to<double>(0.99999999999995, "9.9999999999995ᴇ-1",
                                     ScientificMode, 14);
  assert_float_serializes_to<double>(0.00000000099999999999995,
                                     "9.9999999999995ᴇ-10", DecimalMode, 14);
  assert_float_serializes_to<double>(
      0.0000000009999999999901200121020102010201201201021099995,
      "9.9999999999012ᴇ-10", DecimalMode, 14);
  assert_float_serializes_to<float>(1.2345E-1, "0.12345", DecimalMode);
  assert_float_serializes_to<float>(1, "1", DecimalMode);
  assert_float_serializes_to<float>(0.9999999999999995, "1", DecimalMode);
  assert_float_serializes_to<float>(1.2345E6, "1234500", DecimalMode);
  assert_float_serializes_to<float>(-1.2345E6, "-1234500", DecimalMode);
  assert_float_serializes_to<float>(0.0000009999999999999995, "1ᴇ-6",
                                    DecimalMode);
  assert_float_serializes_to<float>(-1.2345E-1, "-0.12345", DecimalMode);

  assert_float_serializes_to<double>(INFINITY, "∞", DecimalMode);
  assert_float_serializes_to<float>(0.0f, "0", DecimalMode);
  assert_float_serializes_to<float>(NAN, "undef", DecimalMode);
}

QUIZ_CASE(pcj_serialization_expression) {
  using namespace Poincare;
  // Buffer is too small
  constexpr size_t bufferSize = 5;
  char buffer[bufferSize];
  size_t result = UserExpression::Builder(KCos(1000000_e)).serialize(buffer);
  quiz_assert(result == LayoutHelpers::k_bufferOverflow);

  assert_expression_serializes_to(
      UserExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)), "(2×π)/3", false);
  assert_expression_serializes_to(
      UserExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)), "2π/3", true);
  assert_expression_serializes_to(
      UserExpression::Builder(KMult(KDiv(π_e, 2_e), e_e)), "π/2×e", true);
}

QUIZ_CASE(pcj_serialization_expression_or_float) {
  using namespace Poincare;

  assert_expression_or_float_serializes_to(ExpressionOrFloat(), "undef");

  assert_expression_or_float_serializes_to(ExpressionOrFloat(0.123f), "0.123");
  assert_expression_or_float_serializes_to(ExpressionOrFloat(-2.5f), "-2.5");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(static_cast<float>(M_PI)), "3.141593");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(static_cast<float>(M_PI)), "3.142",
      Preferences::PrintFloatMode::Decimal,
      Preferences::ShortNumberOfSignificantDigits);

  auto approximationFunction = [](Expression expression) {
    return expression.approximateToRealScalar<float>(
        Preferences::AngleUnit::Radian, Preferences::ComplexFormat::Real);
  };

  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(UserExpression::Builder(π_e),
                                 approximationFunction),
      "π");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(
          UserExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)),
          approximationFunction),
      "2π/3");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(
          UserExpression::Builder(KDiv(KMult(2_e, π_e), 30_e)),
          approximationFunction),
      "2π/30");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(
          UserExpression::Builder(KMult(-1_e, KDiv(KMult(2_e, π_e), 30_e))),
          approximationFunction),
      "-0.2094395");

  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(
          UserExpression::Builder(KDiv(KMult(10_e, π_e), 3_e)),
          approximationFunction),
      "", Preferences::PrintFloatMode::Decimal,
      Preferences::VeryLargeNumberOfSignificantDigits, 4);
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(
          UserExpression::Builder(KDiv(KMult(10_e, π_e), 3_e)),
          approximationFunction),
      "10.5", Preferences::PrintFloatMode::Decimal,
      Preferences::VeryShortNumberOfSignificantDigits, 4);

  assert_expression_or_float_serializes_to(
      ExpressionOrFloat::Builder(
          UserExpression::Builder(KSin(KSin(KSin(KSin(1_e))))),
          approximationFunction),
      "0.6275718", Preferences::PrintFloatMode::Decimal);
}

QUIZ_CASE(pcj_serialization_based_integer) {
  assert_expression_serializes_to(
      23_e, "23", Poincare::Preferences::PrintFloatMode::Scientific, 7,
      OMG::Base::Decimal);
  assert_expression_serializes_to(
      23_e, "0b10111", Poincare::Preferences::PrintFloatMode::Scientific, 7,
      OMG::Base::Binary);
  assert_expression_serializes_to(
      23_e, "0x17", Poincare::Preferences::PrintFloatMode::Scientific, 7,
      OMG::Base::Hexadecimal);
}

QUIZ_CASE(pcj_serialization_rational) {
  assert_beautifies_serializes_to(2_e / 3_e, "2/3");

  Tree* e1 = parse("123456789101112131");
  Tree* e2 = Rational::Push(12345678910111213_e, e1);
  assert_beautifies_serializes_to(e2, "12345678910111213/123456789101112131");

  const char* buffer =
      "123456789112345678921234567893123456789412345678951234567896123456789612"
      "345678971234567898123456789912345678901234567891123456789212345678931234"
      "567894123456789512345678961234567896123456789712345678981234567899123456"
      "789";
  Tree* e3 = parse(buffer);
  Tree* e4 = Rational::Push(e3, 1_e);
  assert_beautifies_serializes_to(e4, buffer);

  assert_beautifies_serializes_to(-2_e / 3_e, "-2/3");
  assert_beautifies_serializes_to(2345678909876_e, "2345678909876");
  Tree* e5 = Rational::Push(-2345678909876_e, 5_e);
  assert_beautifies_serializes_to(e5, "-2345678909876/5");

  Tree* e6 = parse(MaxIntegerString());
  assert_beautifies_serializes_to(e6, MaxIntegerString());

  flush_stack();  // TODO: should be done after each quiz_case
}

QUIZ_CASE(pcj_serialization_decimal) {
  Shared::GlobalContext ctx;
  const Tree* d0 = KOpposite(KDecimal(123456789_e, -22_e));
  assert_expression_serializes_to(d0, "-1.23456789ᴇ30", ScientificMode);
  assert_expression_serializes_to(d0, "-1.23456789ᴇ30", DecimalMode);
  assert_expression_serializes_to(d0, "-1.23456789ᴇ30", EngineeringMode);
  const Tree* d1 = KDecimal(123456789_e, -22_e);
  assert_expression_serializes_to(d1, "1.23456789ᴇ30", ScientificMode);
  assert_expression_serializes_to(d1, "1.23456789ᴇ30", DecimalMode);
  assert_expression_serializes_to(d1, "1.23456789ᴇ30", EngineeringMode);
  const Tree* d2 = KOpposite(KDecimal(12345_e, 7_e));
  assert_expression_serializes_to(d2, "-0.0012345", DecimalMode);
  assert_expression_serializes_to(d2, "-1.2345ᴇ-3", ScientificMode);
  assert_expression_serializes_to(d2, "-1.2345ᴇ-3", EngineeringMode);
  const Tree* d4 = KDecimal(12345_e, 7_e);
  assert_expression_serializes_to(d4, "0.0012345", DecimalMode);
  assert_expression_serializes_to(d4, "1.2345ᴇ-3", ScientificMode);
  assert_expression_serializes_to(d4, "1.2345ᴇ-3", EngineeringMode);
  const Tree* d5 = KDecimal(12345_e, 1_e);
  assert_expression_serializes_to(d5, "1234.5", DecimalMode);
  assert_expression_serializes_to(d5, "1.2345ᴇ3", ScientificMode);
  assert_expression_serializes_to(d5, "1.2345ᴇ3", EngineeringMode);
  const Tree* d6 = KOpposite(KDecimal(12345_e, 1_e));
  assert_expression_serializes_to(d6, "-1234.5", DecimalMode);
  assert_expression_serializes_to(d6, "-1.2345ᴇ3", ScientificMode);
  assert_expression_serializes_to(d6, "-1.2345ᴇ3", EngineeringMode);
  const Tree* d7 = KDecimal(12345_e, -2_e);
  assert_expression_serializes_to(d7, "1234500", DecimalMode);
  assert_expression_serializes_to(d7, "1.2345ᴇ6", ScientificMode);
  assert_expression_serializes_to(d7, "1.2345ᴇ6", EngineeringMode);
  const Tree* d8 = KOpposite(KDecimal(12345_e, -2_e));
  assert_expression_serializes_to(d8, "-1234500", DecimalMode);
  assert_expression_serializes_to(d7, "1.2345ᴇ6", ScientificMode);
  assert_expression_serializes_to(d7, "1.2345ᴇ6", EngineeringMode);
  const Tree* d9 = KOpposite(KDecimal(12345_e, 5_e));
  assert_expression_serializes_to(d9, "-0.12345", DecimalMode);
  assert_expression_serializes_to(d9, "-1.2345ᴇ-1", ScientificMode);
  assert_expression_serializes_to(d9, "-123.45ᴇ-3", EngineeringMode);
  const Tree* d10 = KDecimal(12345_e, 5_e);
  assert_expression_serializes_to(d10, "0.12345", DecimalMode);
  assert_expression_serializes_to(d10, "1.2345ᴇ-1", ScientificMode);
  assert_expression_serializes_to(d10, "123.45ᴇ-3", EngineeringMode);

  assert_expression_serializes_to(0.25_e, "250ᴇ-3", EngineeringMode);
  assert_expression_serializes_to(KOpposite(KDecimal(123456789_e, -22_e)),
                                  "-1.23456789ᴇ30", ScientificMode);
  assert_expression_serializes_to(KDecimal(123456789_e, -22_e), "1.23456789ᴇ30",
                                  ScientificMode);
  assert_expression_serializes_to(KOpposite(KDecimal(123456789_e, 38_e)),
                                  "-1.23456789ᴇ-30", ScientificMode);
  assert_expression_serializes_to(KOpposite(KDecimal(12345_e, 7_e)),
                                  "-0.0012345", DecimalMode);
  assert_expression_serializes_to(KDecimal(12345_e, 7_e), "0.0012345",
                                  DecimalMode);
  assert_expression_serializes_to(KDecimal(12345_e, 1_e), "1234.5",
                                  DecimalMode);
  assert_expression_serializes_to(KOpposite(KDecimal(12345_e, 1_e)), "-1234.5",
                                  DecimalMode);
  assert_expression_serializes_to(KDecimal(12345_e, -2_e), "1234500",
                                  DecimalMode);
  assert_expression_serializes_to(KOpposite(KDecimal(12345_e, -2_e)),
                                  "-1234500", DecimalMode);
  assert_expression_serializes_to(KOpposite(KDecimal(12345_e, 5_e)), "-0.12345",
                                  DecimalMode);
  assert_expression_serializes_to(KDecimal(12345_e, 5_e), "0.12345",
                                  DecimalMode);
  assert_expression_serializes_to(1.0_e, "1");
  assert_expression_parses_and_serializes_to(
      "0.99999999999999999999999999999996", "1", &ctx);
  assert_expression_parses_and_serializes_to(
      "0.999999999999999999999999999995", "9.99999999999999999999999999995ᴇ-1",
      &ctx, ScientificMode, 14);
  assert_expression_parses_and_serializes_to(
      "0.000000999999999999999999999999999995",
      "9.99999999999999999999999999995ᴇ-7", &ctx, ScientificMode, 14);
  assert_expression_parses_and_serializes_to(
      "0.0000009999999999999999999999999999995", "1ᴇ-6", &ctx, DecimalMode);
  assert_expression_parses_and_serializes_to(
      "0.0000009999999999999999999999999901200121020102010201201201021099995",
      "9.9999999999999999999999999012ᴇ-7", &ctx, DecimalMode, 14);
  assert_expression_parses_and_serializes_to("99999999999999999999999999999.53",
                                             "99999999999999999999999999999.5",
                                             &ctx, DecimalMode, 14);
  assert_expression_parses_and_serializes_to(
      "999999999999999999999999999999.54", "1ᴇ30", &ctx, DecimalMode, 14);
  assert_expression_parses_and_serializes_to(
      "9999999999999999999999999999999.54", "1ᴇ31", &ctx, DecimalMode, 14);
  assert_expression_parses_and_serializes_to(
      "99999999999999999999999999999999.54", "1ᴇ32", &ctx, DecimalMode, 14);
  assert_expression_parses_and_serializes_to(
      "-9.702365051313ᴇ-297", "-9.702365051313ᴇ-297", &ctx, DecimalMode, 14);

  // Engineering notation
  assert_expression_serializes_to(0.0_e, "0", EngineeringMode);
  assert_expression_serializes_to(10.0_e, "10.0", EngineeringMode);
  assert_expression_serializes_to(100.0_e, "100.0", EngineeringMode);
  assert_expression_serializes_to(1000.0_e, "1.0ᴇ3", EngineeringMode);
  assert_expression_serializes_to(1234.0_e, "1.234ᴇ3", EngineeringMode);
  assert_expression_serializes_to(KOpposite(0.1_e), "-100ᴇ-3", EngineeringMode);
  assert_expression_serializes_to(KOpposite(0.01_e), "-10ᴇ-3", EngineeringMode);
  assert_expression_serializes_to(KOpposite(0.001_e), "-1ᴇ-3", EngineeringMode);
}

QUIZ_CASE(pcj_serialization_division) {
  assert_beautifies_serializes_to(KDiv(-2_e, π_e), "(-2)/π");
  assert_beautifies_serializes_to(KDiv(π_e, -2_e), "π/(-2)");
  assert_expression_serializes_to(KDiv(2_e, 3_e), "2/3");
  assert_beautifies_serializes_to(KDiv(KDiv(2_e, 3_e), π_e), "(2/3)/π");
  assert_beautifies_serializes_to(KDiv(KAdd(1_e, 2_e), π_e), "(1+2)/π");
  assert_beautifies_serializes_to(KDiv(KSub(2_e, 1_e), π_e), "(2-1)/π");
  assert_beautifies_serializes_to(KDiv(KMult(1_e, 2_e), π_e), "(1×2)/π");
  assert_beautifies_serializes_to(KDiv(KDiv(2_e, 1_e), π_e), "(2/1)/π");
  assert_beautifies_serializes_to(KDiv(2_e, KDiv(1_e, π_e)), "2/(1/π)");
  assert_beautifies_serializes_to(KDiv(KDiv(2_e, 3_e), KDiv(1_e, π_e)),
                                  "(2/3)/(1/π)");
  assert_beautifies_serializes_to(KDiv(KOpposite(2_e), π_e), "(-2)/π");
}

QUIZ_CASE(pcj_serialization_factorial) {
  assert_expression_serializes_to(KFact(KDiv(2_e, 3_e)), "(2/3)!");
  assert_expression_serializes_to(KFact(KDiv(π_e, 3_e)), "(π/3)!");
  assert_expression_serializes_to(KFact(KPow(π_e, 3_e)), "(π^3)!");
}

QUIZ_CASE(pcj_serialization_percent) {
  assert_expression_serializes_to(KPercentSimple(KDiv(2_e, 3_e)), "(2/3)%");
  assert_expression_serializes_to(KPercentSimple(KDiv(π_e, 3_e)), "(π/3)%");
  assert_expression_serializes_to(KPercentSimple(KPow(π_e, 3_e)), "(π^3)%");
  assert_expression_serializes_to(
      KPercentAddition(KPercentAddition(100_e, 20_e), KOpposite(30_e)),
      "(100↗20%)↘30%");
  assert_expression_serializes_to(KPow(π_e, KPercentAddition(100_e, 20_e)),
                                  "π^(100↗20%)");
}

QUIZ_CASE(pcj_serialization_power) {
  assert_expression_serializes_and_parses_to(
      KPow(2_e, KEqual(3_e, KSub(4_e, 5_e))),
      KPow(2_e, KParentheses(KEqual(3_e, KSub(4_e, 5_e)))));
  assert_expression_serializes_and_parses_to(
      KPow(2_e, KPercentSimple(KPow(3_e, 4_e))),
      KPow(2_e, KPercentSimple(KParentheses(KPow(3_e, 4_e)))));
  assert_expression_serializes_and_parses_to(
      KPercentSimple(KPow(2_e, KLogicalOr(3_e, 4_e))),
      KPercentSimple(
          KParentheses(KPow(2_e, KParentheses(KLogicalOr(3_e, 4_e))))));
  assert_expression_serializes_and_parses_to(
      KAbs(KPercentAddition(
          0_e, KDiv(0_e, KLogicalAnd(
                             "r"_e, KMult("o"_e, "m0"_e, KParentheses(0_e)))))),
      KAbs(KPercentAddition(
          0_e,
          KParentheses(KDiv(
              0_e, KParentheses(KLogicalAnd(
                       "r"_e, KMult("o"_e, "m0"_e, KParentheses(0_e)))))))));
}

QUIZ_CASE(pcj_serialization_derivative) {
  Shared::GlobalContext ctx;
  assert_expression_serializes_to(KDiff("x"_e, "x"_e, 1_e, KFun<"f">("x"_e)),
                                  "diff(f(x),x,x)");
  assert_expression_serializes_to(KDiff("x"_e, "x"_e, 2_e, KFun<"f">("x"_e)),
                                  "diff(f(x),x,x,2)");
  assert_expression_serializes_to(KDiff("x"_e, "x"_e, 3_e, KFun<"f">("x"_e)),
                                  "diff(f(x),x,x,3)");
  assert_expression_serializes_to(
      KDiff(Derivation::k_functionDerivativeVariable, "x"_e, 1_e,
            KFun<"f">(Derivation::k_functionDerivativeVariable)),
      "f'(x)");
  assert_expression_serializes_to(
      KDiff(Derivation::k_functionDerivativeVariable, "x"_e, 2_e,
            KFun<"f">(Derivation::k_functionDerivativeVariable)),
      "f\"(x)");
  assert_expression_serializes_to(
      KDiff(Derivation::k_functionDerivativeVariable, "x"_e, 3_e,
            KFun<"f">(Derivation::k_functionDerivativeVariable)),
      "f^(3)(x)");
  assert_expression_parses_and_serializes_to("f'(x)", "f×_'×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f\"(x)", "f×_\"×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f''(x)", "f×_'×_'×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f'\"(x)", "f×_'×_\"×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f\"\"(x)", "f×_\"×_\"×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f^(1)(x)", "f^(1)×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f^(2)(x)", "f^(2)×(x)", &ctx);
  assert_expression_parses_and_serializes_to("f^(3)(x)", "f^(3)×(x)", &ctx);
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      "f", "func", "", 0);
  assert_expression_parses_and_serializes_to_itself("f(x)", &ctx);
  assert_expression_parses_and_serializes_to_itself("f'(x+1)", &ctx);
  assert_expression_parses_and_serializes_to_itself("f\"(x+1)", &ctx);
  assert_expression_parses_and_serializes_to_itself("f^(3)(x+1)", &ctx);
  assert_expression_parses_and_serializes_to_itself("diff(f(x),x,a)", &ctx);
  assert_expression_parses_and_serializes_to_itself("diff(f(x),x,a,2)", &ctx);
  assert_expression_parses_and_serializes_to_itself("diff(f(x),x,a,3)", &ctx);
  assert_expression_parses_and_serializes_to("f''(x)", "f\"(x)", &ctx);
  assert_expression_parses_and_serializes_to("f'''(x)", "f^(3)(x)", &ctx);
  assert_expression_parses_and_serializes_to("f'\"(x)", "f^(3)(x)", &ctx);
  assert_expression_parses_and_serializes_to("f\"\"(x)", "f^(4)(x)", &ctx);
  assert_expression_parses_and_serializes_to("f^(1)(x)", "f'(x)", &ctx);
  assert_expression_parses_and_serializes_to("f^(2)(x)", "f\"(x)", &ctx);
  assert_expression_parses_and_serializes_to("f^(3)(x)", "f^(3)(x)", &ctx);
}
