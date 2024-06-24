#include <poincare/old/addition.h>
#include <poincare/old/based_integer.h>
#include <poincare/old/binomial_coefficient.h>
#include <poincare/old/comparison.h>
#include <poincare/old/constant.h>
#include <poincare/old/cosine.h>
#include <poincare/old/decimal.h>
#include <poincare/old/derivative.h>
#include <poincare/old/division.h>
#include <poincare/old/factorial.h>
#include <poincare/old/integral.h>
#include <poincare/old/logarithm.h>
#include <poincare/old/matrix.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/nth_root.h>
#include <poincare/old/piecewise_operator.h>
#include <poincare/old/poincare_expressions.h>
#include <poincare/old/power.h>
#include <poincare/src/expression/conversion.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parser.h>

#include "old/helper.h"

using namespace Poincare;
using namespace Poincare::Internal::KTrees;

void assert_layout_is_not_parsed(Layout l) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serializeForParsing(buffer, bufferSize);
  Expression e = Expression::Parse(buffer, nullptr, false);
  quiz_assert_print_if_failure(e.isUninitialized(), buffer);
}

// TODO_PCJ this parses correctly in pcj and should not
QUIZ_CASE_DISABLED(poincare_layout_to_expression_unparsable) {
  {
    /*   d     |
     * -----(x)|
     * d 1+x   |1+x=3
     */
    Layout l = KRackL(KDiffL("1+x"_l, "3"_l, "x"_l));
    assert_layout_is_not_parsed(l);
  }
  {
    /*   d^2     |
     * -------(x)|
     * d 1+x^2   |1+x=3
     */
    Layout l = KRackL(KNthDiffL("1+x"_l, "3"_l, "2"_l, "x"_l));
    assert_layout_is_not_parsed(l);
  }
}

void assert_parsed_layout_is(Layout l, Poincare::OExpression r) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serializeForParsing(buffer, bufferSize);

  Internal::Tree* ej = Internal::Parser::Parse(l.tree()->clone(), nullptr);
  quiz_assert_print_if_failure(ej, buffer);
  Internal::Tree* rj = Internal::FromPoincareExpression(r);
  quiz_assert_print_if_failure(rj, buffer);
  quiz_assert_print_if_failure(ej->treeIsIdenticalTo(rj), buffer);
  Internal::TreeStack::SharedTreeStack->flush();
}

OMatrix BuildOneChildMatrix(OExpression entry) {
  OMatrix m = OMatrix::Builder();
  m.addChildAtIndexInPlace(entry, 0, 0);
  return m;
}

QUIZ_CASE(poincare_layout_to_expression_parsable) {
  Layout l;
  OExpression e;

  // 1+2
  l = "1+2"_l;
  e = Addition::Builder(BasedInteger::Builder(1), BasedInteger::Builder(2));
  assert_parsed_layout_is(l, e);

  // |3+3/6|
  l = KRackL(KAbsL("3+"_l ^ KFracL("3"_l, "6"_l)));
  e = AbsoluteValue::Builder(Addition::Builder(
      BasedInteger::Builder(3),
      Division::Builder(BasedInteger::Builder(3), BasedInteger::Builder(6))));
  assert_parsed_layout_is(l, e);

  // binCoef(4,5)
  l = KRackL(KBinomialL("4"_l, "5"_l));
  e = BinomialCoefficient::Builder(BasedInteger::Builder(4),
                                   BasedInteger::Builder(5));
  assert_parsed_layout_is(l, e);

  // ceil(4.6)
  l = KRackL(KCeilL("4.6"_l));
  e = Ceiling::Builder(Decimal::Builder(4.6));
  assert_parsed_layout_is(l, e);

  // floor(7.2)
  l = KRackL(KFloorL("7.2"_l));
  e = Floor::Builder(Decimal::Builder(7.2));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)
  l = "2"_l ^ KSuperscriptL("3+4"_l);
  e = Power::Builder(
      BasedInteger::Builder(2),
      Addition::Builder(BasedInteger::Builder(3), BasedInteger::Builder(4)));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)!
  l = "2"_l ^ KSuperscriptL("3+4"_l) ^ "!"_l;
  e = Factorial::Builder(Power::Builder(
      BasedInteger::Builder(2),
      Addition::Builder(BasedInteger::Builder(3), BasedInteger::Builder(4))));
  assert_parsed_layout_is(l, e);

  // 2^3^4
  l = "2"_l ^ KSuperscriptL("3"_l) ^ KSuperscriptL("4"_l);
  e = Power::Builder(
      BasedInteger::Builder(2),
      Power::Builder(BasedInteger::Builder(3), BasedInteger::Builder(4)));
  assert_parsed_layout_is(l, e);

  // log_3(2)
  l = "log"_l ^ KSubscriptL("3"_l) ^ KParenthesisL("2"_l);
  e = Logarithm::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  /* 3
   *  log(2) */
  l = KPrefixSuperscriptL("3"_l) ^ "log"_l ^ KParenthesisL("2"_l);
  e = Logarithm::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  // root(5,3)
  l = KRackL(KRootL("5"_l, "3"_l));
  e = NthRoot::Builder(BasedInteger::Builder(5), BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  // int(7, x, 4, 5)
  l = KRackL(KIntegralL("x"_l, "4"_l, "5"_l, "7"_l));
  e = Integral::Builder(BasedInteger::Builder(7), Symbol::Builder('x'),
                        BasedInteger::Builder(4), BasedInteger::Builder(5));
  assert_parsed_layout_is(l, e);

  // 2^2 !
  l = "2"_l ^ KSuperscriptL("2"_l) ^ "!"_l;
  e = Factorial::Builder(
      Power::Builder(BasedInteger::Builder(2), BasedInteger::Builder(2)));
  assert_parsed_layout_is(l, e);

  // 5* 6/(7+5) *3
  l = "5"_l ^ KFracL("6"_l, "7+5"_l) ^ "3"_l;
  e = Multiplication::Builder(
      BasedInteger::Builder(5),
      Division::Builder(BasedInteger::Builder(6),
                        Addition::Builder(BasedInteger::Builder(7),
                                          BasedInteger::Builder(5))),
      BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  // [[3^2!, 7][4,5]
  l = KRackL(
      KMatrix2x2L("3"_l ^ KSuperscriptL("2"_l) ^ "!"_l, "7"_l, "4"_l, "5"_l));
  OMatrix m = BuildOneChildMatrix(Factorial::Builder(
      Power::Builder(BasedInteger::Builder(3), BasedInteger::Builder(2))));
  m.addChildAtIndexInPlace(BasedInteger::Builder(7), 1, 1);
  m.addChildAtIndexInPlace(BasedInteger::Builder(4), 2, 2);
  m.addChildAtIndexInPlace(BasedInteger::Builder(5), 3, 3);
  m.setDimensions(2, 2);
  e = m;
  assert_parsed_layout_is(l, e);

  // 2^det([[3!, 7][4,5])
  l = "2"_l ^ KSuperscriptL("det"_l ^ KParenthesisL(KRackL(KMatrix2x2L(
                                          "3!"_l, "7"_l, "4"_l, "5"_l))));
  m = BuildOneChildMatrix(Factorial::Builder(BasedInteger::Builder(3)));
  m.addChildAtIndexInPlace(BasedInteger::Builder(7), 1, 1);
  m.addChildAtIndexInPlace(BasedInteger::Builder(4), 2, 2);
  m.addChildAtIndexInPlace(BasedInteger::Builder(5), 3, 3);
  m.setDimensions(2, 2);
  e = Power::Builder(BasedInteger::Builder(2), Determinant::Builder(m));
  assert_parsed_layout_is(l, e);

  // 2e^3
  l = "2e"_l ^ KSuperscriptL("3"_l);
  e = Multiplication::Builder(BasedInteger::Builder(2),
                              Power::Builder(Constant::ExponentialEBuilder(),
                                             BasedInteger::Builder(3)));
  assert_parsed_layout_is(l, e);

  // x|2|
  l = "x"_l ^ KAbsL("2"_l);
  e = Multiplication::Builder(Symbol::Builder('x'),
                              AbsoluteValue::Builder(BasedInteger::Builder(2)));
  assert_parsed_layout_is(l, e);

  // x int(7, x, 4, 5)
  l = "x"_l ^ KIntegralL("x"_l, "4"_l, "5"_l, "7"_l);
  e = Multiplication::Builder(
      Symbol::Builder('x'),
      Integral::Builder(BasedInteger::Builder(7), Symbol::Builder('x'),
                        BasedInteger::Builder(4), BasedInteger::Builder(5)));
  assert_parsed_layout_is(l, e);

  // diff(1/Var, Var, cos(2))
  l = KRackL(KDiffL("Var"_l, "cos(2)"_l, KRackL(KFracL("1"_l, "Var"_l))));
  e = Derivative::Builder(
      Division::Builder(BasedInteger::Builder(1), Symbol::Builder("Var", 3)),
      Symbol::Builder("Var", 3), Cosine::Builder(BasedInteger::Builder(2)),
      Rational::Builder(1));

  // TODO_PCJ: disabled because Var is a multi-letter variable
  // assert_parsed_layout_is(l, e);

  // diff(1/Var, Var, cos(2), 2)
  l = KRackL(
      KNthDiffL("Var"_l, "cos(2)"_l, KRackL(KFracL("1"_l, "Var"_l)), "2"_l));
  e = Derivative::Builder(
      Division::Builder(BasedInteger::Builder(1), Symbol::Builder("Var", 3)),
      Symbol::Builder("Var", 3), Cosine::Builder(BasedInteger::Builder(2)),
      BasedInteger::Builder(2));

  // assert_parsed_layout_is(l, e);

  // Piecewise
  using namespace Poincare::Internal;
  // TODO: this is tedious and fragile
  Tree* p = SharedTreeStack->pushRackLayout(1);
  SharedTreeStack->pushPiecewiseLayout(4, 2);
  "3"_l->clone();
  "2>3"_l->clone();
  "2"_l->clone();
  "2<3"_l->clone();
  "1"_l->clone();
  ""_l->clone();
  ""_l->clone();
  ""_l->clone();
  l = Layout::Builder(p);

  OList args = OList::Builder();
  args.addChildAtIndexInPlace(BasedInteger::Builder(3), 0, 0);
  args.addChildAtIndexInPlace(
      Comparison::Builder(BasedInteger::Builder(2),
                          ComparisonNode::OperatorType::Superior,
                          BasedInteger::Builder(3)),
      1, 1);
  args.addChildAtIndexInPlace(BasedInteger::Builder(2), 2, 2);
  args.addChildAtIndexInPlace(
      Comparison::Builder(BasedInteger::Builder(2),
                          ComparisonNode::OperatorType::Inferior,
                          BasedInteger::Builder(3)),
      3, 3);
  args.addChildAtIndexInPlace(BasedInteger::Builder(1), 4, 4);
  e = PiecewiseOperator::UntypedBuilder(args);

  assert_parsed_layout_is(l, e);
}
