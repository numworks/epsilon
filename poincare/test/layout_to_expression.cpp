#include <poincare_layouts.h>
#include <poincare/addition.h>
#include <poincare/based_integer.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/comparison.h>
#include <poincare/constant.h>
#include <poincare/cosine.h>
#include <poincare/decimal.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/factorial.h>
#include <poincare/integral.h>
#include <poincare/logarithm.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/nth_root.h>
#include <poincare/piecewise_operator.h>
#include <poincare/power.h>
#include "helper.h"

using namespace Poincare;

void assert_layout_is_not_parsed(Layout l) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serializeForParsing(buffer, bufferSize);
  Expression e = Expression::Parse(buffer, nullptr, false);
  quiz_assert_print_if_failure(e.isUninitialized(), buffer);
}

QUIZ_CASE(poincare_layout_to_expression_unparsable) {
  {
    /*   d     |
     * -----(x)|
     * d 1+x   |1+x=3
     */
    Layout l = FirstOrderDerivativeLayout::Builder(
      CodePointLayout::Builder('x'),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('1'),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder('x')
      }),
      CodePointLayout::Builder('3')
    );
    assert_layout_is_not_parsed(l);
  }
  {
    /*   d^2     |
     * -------(x)|
     * d 1+x^2   |1+x=3
     */
    Layout l = HigherOrderDerivativeLayout::Builder(
      CodePointLayout::Builder('x'),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('1'),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder('x')
      }),
      CodePointLayout::Builder('3'),
      CodePointLayout::Builder('2')
    );
    assert_layout_is_not_parsed(l);
  }
}

void assert_parsed_layout_is(Layout l, Poincare::Expression r) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serializeForParsing(buffer, bufferSize);
  Expression e = parse_expression(buffer, nullptr, true);
  quiz_assert_print_if_failure(e.isIdenticalTo(r), buffer);
}

Matrix BuildOneChildMatrix(Expression entry) {
  Matrix m = Matrix::Builder();
  m.addChildAtIndexInPlace(entry, 0, 0);
  return m;
}

QUIZ_CASE(poincare_layout_to_expression_parsable) {
  Layout l;
  Expression e;

  // 1+2
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('1'),
      CodePointLayout::Builder('+'),
      CodePointLayout::Builder('2'));
  e = Addition::Builder(BasedInteger::Builder(1), BasedInteger::Builder(2));
  assert_parsed_layout_is(l, e);

  // |3+3/6|
  l = AbsoluteValueLayout:: Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('3'),
        CodePointLayout::Builder('+'),
        FractionLayout::Builder(
          CodePointLayout::Builder('3'),
          CodePointLayout::Builder('6'))));
  e = AbsoluteValue::Builder(
      Addition::Builder(
        BasedInteger::Builder(3),
        Division::Builder(
          BasedInteger::Builder(3),
          BasedInteger::Builder(6))));
  assert_parsed_layout_is(l, e);

  // binCoef(4,5)
  l = BinomialCoefficientLayout::Builder(
      CodePointLayout::Builder('4'),
      CodePointLayout::Builder('5'));
  e = BinomialCoefficient::Builder(
      BasedInteger::Builder(4),
      BasedInteger::Builder(5));
  assert_parsed_layout_is(l, e);

  // ceil(4.6)
  l = CeilingLayout::Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('4'),
        CodePointLayout::Builder('.'),
        CodePointLayout::Builder('6')));
  e = Ceiling::Builder(
      Decimal::Builder(4.6));
  assert_parsed_layout_is(l, e);

  // floor(7.2)
  l = FloorLayout::Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('7'),
        CodePointLayout::Builder('.'),
        CodePointLayout::Builder('2')));
  e = Floor::Builder(
      Decimal::Builder(7.2));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(
          CodePointLayout::Builder('3'),
          CodePointLayout::Builder('+'),
          CodePointLayout::Builder('4')),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript));
  e = Power::Builder(
      BasedInteger::Builder(2),
      Addition::Builder(
        BasedInteger::Builder(3),
        BasedInteger::Builder(4)));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)!
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(
          CodePointLayout::Builder('3'),
          CodePointLayout::Builder('+'),
          CodePointLayout::Builder('4')),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript),
      CodePointLayout::Builder('!'));
  e = Factorial::Builder(
      Power::Builder(
        BasedInteger::Builder(2),
        Addition::Builder(
          BasedInteger::Builder(3),
          BasedInteger::Builder(4))));
  assert_parsed_layout_is(l, e);


  // log_3(2)
  HorizontalLayout l1 = HorizontalLayout::Builder();
  l1.addChildAtIndex(CodePointLayout::Builder('l'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CodePointLayout::Builder('o'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CodePointLayout::Builder('g'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(VerticalOffsetLayout::Builder(CodePointLayout::Builder('3'), VerticalOffsetLayoutNode::VerticalPosition::Subscript), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(ParenthesisLayout::Builder(CodePointLayout::Builder('2')), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l = l1;
  e = Logarithm::Builder(
      BasedInteger::Builder(2),
      BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  /* 3
   *  log(2) */
  l = HorizontalLayout::Builder({
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('3'), VerticalOffsetLayoutNode::VerticalPosition::Superscript, VerticalOffsetLayoutNode::HorizontalPosition::Prefix),
      StringLayout::Builder("log"),
      ParenthesisLayout::Builder(CodePointLayout::Builder('2'))
      });
  e = Logarithm::Builder(
      BasedInteger::Builder(2),
      BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);


  // root(5,3)
  l = NthRootLayout::Builder(
      CodePointLayout::Builder('5'),
      CodePointLayout::Builder('3'));
  e = NthRoot::Builder(BasedInteger::Builder(5), BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  // int(7, x, 4, 5)
  l = IntegralLayout::Builder(
      CodePointLayout::Builder('7'),
      CodePointLayout::Builder('x'),
      CodePointLayout::Builder('4'),
      CodePointLayout::Builder('5'));
  e = Integral::Builder(
      BasedInteger::Builder(7),
      Symbol::Builder('x'),
      BasedInteger::Builder(4),
      BasedInteger::Builder(5));
  assert_parsed_layout_is(l, e);

  // 2^2 !
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript),
      CodePointLayout::Builder('!'));
  e = Factorial::Builder(
      Power::Builder(
        BasedInteger::Builder(2),
        BasedInteger::Builder(2)));
  assert_parsed_layout_is(l, e);

  // 5* 6/(7+5) *3
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('5'),
      FractionLayout::Builder(
        CodePointLayout::Builder('6'),
        HorizontalLayout::Builder(
          CodePointLayout::Builder('7'),
          CodePointLayout::Builder('+'),
          CodePointLayout::Builder('5'))),
      CodePointLayout::Builder('3'));
  e = Multiplication::Builder(
      BasedInteger::Builder(5),
      Division::Builder(
        BasedInteger::Builder(6),
        Addition::Builder(
          BasedInteger::Builder(7),
          BasedInteger::Builder(5))),
      BasedInteger::Builder(3));
  assert_parsed_layout_is(l, e);

  // [[3^2!, 7][4,5]
  l = MatrixLayout::Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('3'),
        VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('2'),
          VerticalOffsetLayoutNode::VerticalPosition::Superscript),
        CodePointLayout::Builder('!')),
      CodePointLayout::Builder('7'),
      CodePointLayout::Builder('4'),
      CodePointLayout::Builder('5'));
  Matrix m = BuildOneChildMatrix(
      Factorial::Builder(
        Power::Builder(
          BasedInteger::Builder(3),
          BasedInteger::Builder(2))));
  m.addChildAtIndexInPlace(BasedInteger::Builder(7), 1, 1);
  m.addChildAtIndexInPlace(BasedInteger::Builder(4), 2, 2);
  m.addChildAtIndexInPlace(BasedInteger::Builder(5), 3, 3);
  m.setDimensions(2,2);
  e = m;
  assert_parsed_layout_is(l, e);

  // 2^det([[3!, 7][4,5])
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        MatrixLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('3'),
            CodePointLayout::Builder('!')),
          CodePointLayout::Builder('7'),
          CodePointLayout::Builder('4'),
          CodePointLayout::Builder('5')),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript));
  m = BuildOneChildMatrix(
      Factorial::Builder(
        BasedInteger::Builder(3)));
  m.addChildAtIndexInPlace(BasedInteger::Builder(7), 1, 1);
  m.addChildAtIndexInPlace(BasedInteger::Builder(4), 2, 2);
  m.addChildAtIndexInPlace(BasedInteger::Builder(5), 3, 3);
  m.setDimensions(2,2);
  e = Power::Builder(BasedInteger::Builder(2), m);
  assert_parsed_layout_is(l, e);

  // 2e^3
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      CodePointLayout::Builder('e'),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('3'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript));
  e = Multiplication::Builder(BasedInteger::Builder(2),Power::Builder(Constant::Builder("e"), BasedInteger::Builder(3)));
  assert_parsed_layout_is(l, e);

  // x|2|
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('x'),
      AbsoluteValueLayout::Builder(
        CodePointLayout::Builder('2')));
  e = Multiplication::Builder(
      Symbol::Builder('x'),
      AbsoluteValue::Builder(BasedInteger::Builder(2)));
  assert_parsed_layout_is(l, e);

  // x int(7, x, 4, 5)
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('x'),
      IntegralLayout::Builder(
        CodePointLayout::Builder('7'),
        CodePointLayout::Builder('x'),
        CodePointLayout::Builder('4'),
        CodePointLayout::Builder('5')));
  e = Multiplication::Builder(
      Symbol::Builder('x'),
      Integral::Builder(
        BasedInteger::Builder(7),
        Symbol::Builder('x'),
        BasedInteger::Builder(4),
        BasedInteger::Builder(5)));
  assert_parsed_layout_is(l, e);

  // diff(1/Var, Var, cos(2))
  l = FirstOrderDerivativeLayout::Builder(
      FractionLayout::Builder(
        CodePointLayout::Builder('1'),
        HorizontalLayout::Builder({
          CodePointLayout::Builder('V'),
          CodePointLayout::Builder('a'),
          CodePointLayout::Builder('r')
          })),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('V'),
        CodePointLayout::Builder('a'),
        CodePointLayout::Builder('r')
        }),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('c'),
        CodePointLayout::Builder('o'),
        CodePointLayout::Builder('s'),
        CodePointLayout::Builder('('),
        CodePointLayout::Builder('2'),
        CodePointLayout::Builder(')'),
        }));
  e = Derivative::Builder(
      Division::Builder(
        BasedInteger::Builder(1),
        Symbol::Builder("Var", 3)),
      Symbol::Builder("Var", 3),
      Cosine::Builder(
        BasedInteger::Builder(2)),
      Rational::Builder(1)
      );

  assert_parsed_layout_is(l, e);

    // diff(1/Var, Var, cos(2), 2)
  l = HigherOrderDerivativeLayout::Builder(
      FractionLayout::Builder(
        CodePointLayout::Builder('1'),
        HorizontalLayout::Builder({
          CodePointLayout::Builder('V'),
          CodePointLayout::Builder('a'),
          CodePointLayout::Builder('r')
          })),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('V'),
        CodePointLayout::Builder('a'),
        CodePointLayout::Builder('r')
        }),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('c'),
        CodePointLayout::Builder('o'),
        CodePointLayout::Builder('s'),
        CodePointLayout::Builder('('),
        CodePointLayout::Builder('2'),
        CodePointLayout::Builder(')'),
        }),
      CodePointLayout::Builder('2'));
  e = Derivative::Builder(
      Division::Builder(
        BasedInteger::Builder(1),
        Symbol::Builder("Var", 3)),
      Symbol::Builder("Var", 3),
      Cosine::Builder(
        BasedInteger::Builder(2)),
      BasedInteger::Builder(2)
      );

  assert_parsed_layout_is(l, e);

  // Piecewise
  PiecewiseOperatorLayout p = PiecewiseOperatorLayout::Builder();
  p.addRow(CodePointLayout::Builder('3'),HorizontalLayout::Builder(CodePointLayout::Builder('2'), CodePointLayout::Builder('>'), CodePointLayout::Builder('3')));
  p.addRow(CodePointLayout::Builder('2'),HorizontalLayout::Builder(CodePointLayout::Builder('2'), CodePointLayout::Builder('<'), CodePointLayout::Builder('3')));
  p.addRow(CodePointLayout::Builder('1'));

  List args = List::Builder();
  args.addChildAtIndexInPlace(BasedInteger::Builder(3),0,0);
  args.addChildAtIndexInPlace(Comparison::Builder(BasedInteger::Builder(2),ComparisonNode::OperatorType::Superior,BasedInteger::Builder(3)),1,1);
  args.addChildAtIndexInPlace(BasedInteger::Builder(2),2,2);
  args.addChildAtIndexInPlace(Comparison::Builder(BasedInteger::Builder(2),ComparisonNode::OperatorType::Inferior,BasedInteger::Builder(3)),3,3);
  args.addChildAtIndexInPlace(BasedInteger::Builder(1),4,4);
  e = PiecewiseOperator::UntypedBuilder(args);

  assert_parsed_layout_is(p, e);
}
