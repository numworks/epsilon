#include <quiz.h>
#include <poincare.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_function) {
  assert_parsed_expression_type("abs(-1)", Expression::Type::AbsoluteValue);
  assert_parsed_expression_type("arg(2+I)", Expression::Type::ComplexArgument);
  assert_parsed_expression_type("binomial(10, 4)", Expression::Type::BinomialCoefficient);
  assert_parsed_expression_type("ceil(0.2)", Expression::Type::Ceiling);
  assert_parsed_expression_type("diff(2*x, 2)", Expression::Type::Derivative);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_type("dim([[2]])", Expression::Type::MatrixDimension);
  assert_parsed_expression_type("det([[1,2,3][4,5,6][7,8,9]])", Expression::Type::Determinant);
#endif
  assert_parsed_expression_type("confidence(0.1, 100)", Expression::Type::ConfidenceInterval);
  assert_parsed_expression_type("conj(2)", Expression::Type::Conjugate);
  assert_parsed_expression_type("floor(2.3)", Expression::Type::Floor);
  assert_parsed_expression_type("frac(2.3)", Expression::Type::FracPart);
  assert_parsed_expression_type("gcd(2,3)", Expression::Type::GreatCommonDivisor);
  assert_parsed_expression_type("im(2+I)", Expression::Type::ImaginaryPart);
  assert_parsed_expression_type("int(x, 2, 3)", Expression::Type::Integral);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_type("inverse([[1,2,3][4,5,6][7,8,9]])", Expression::Type::MatrixInverse);
#endif
  assert_parsed_expression_type("lcm(2,3)", Expression::Type::LeastCommonMultiple);
  assert_parsed_expression_type("ln(2)", Expression::Type::NaperianLogarithm);
  assert_parsed_expression_type("log(2)", Expression::Type::Logarithm);
  assert_parsed_expression_type("permute(10, 4)", Expression::Type::PermuteCoefficient);
  assert_parsed_expression_type("prediction(0.1, 100)", Expression::Type::ConfidenceInterval);
  assert_parsed_expression_type("prediction95(0.1, 100)", Expression::Type::PredictionInterval);
  assert_parsed_expression_type("product(n, 4, 10)", Expression::Type::Product);
  assert_parsed_expression_type("quo(29, 10)", Expression::Type::DivisionQuotient);
  assert_parsed_expression_type("re(2+I)", Expression::Type::ReelPart);
  assert_parsed_expression_type("rem(29, 10)", Expression::Type::DivisionRemainder);
  assert_parsed_expression_type("root(2,3)", Expression::Type::NthRoot);
  assert_parsed_expression_type("R(2)", Expression::Type::SquareRoot);
  assert_parsed_expression_type("round(2,3)", Expression::Type::Round);
  assert_parsed_expression_type("sum(n, 4, 10)", Expression::Type::Sum);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_type("trace([[1,2,3][4,5,6][7,8,9]])", Expression::Type::MatrixTrace);
  assert_parsed_expression_type("transpose([[1,2,3][4,5,6][7,8,9]])", Expression::Type::MatrixTranspose);
#endif
  assert_parsed_expression_type("6!", Expression::Type::Factorial);
}


QUIZ_CASE(poincare_function_evaluate) {
  Complex<double> a0[1] = {Complex<double>::Float(1.0)};
  assert_parsed_expression_evaluates_to("abs(-1)", a0);

  Complex<float> a1[1] = {Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluates_to("abs(3+2I)", a1);

  Complex<double> a2[4] = {Complex<double>::Float(1.0), Complex<double>::Float(2.0), Complex<double>::Float(3.0), Complex<double>::Float(4.0)};
  assert_parsed_expression_evaluates_to("abs([[1,-2][3,-4]])", a2, 2, 2);

  Complex<float> a3[4] = {Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f)), Complex<float>::Float(std::sqrt(3.0f*3.0f+4.0f*4.0f)), Complex<float>::Float(std::sqrt(5.0f*5.0f+2.0f*2.0f)), Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluates_to("abs([[3+2I,3+4I][5+2I,3+2I]])", a3, 2, 2);

  Complex<double> b[1] = {Complex<double>::Float(210.0)};
  assert_parsed_expression_evaluates_to("binomial(10, 4)", b);

  Complex<float> c[1] = {Complex<float>::Float(1.0f)};
  assert_parsed_expression_evaluates_to("ceil(0.2)", c);

  Complex<double> d[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("diff(2*x, 2)", d);

#if MATRICES_ARE_DEFINED
  Complex<float> e[1] = {Complex<float>::Float(126.0f)};
  assert_parsed_expression_evaluates_to("det([[1,23,3][4,5,6][7,8,9]])", e);
#endif

  Complex<double> f[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("floor(2.3)", f);

  Complex<float> g[1] = {Complex<float>::Float(0.3f)};
  assert_parsed_expression_evaluates_to("frac(2.3)", g);

  Complex<double> h[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("gcd(234,394)", h);

  Complex<float> i[1] = {Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("im(2+3I)", i);

  Complex<double> j[1] = {Complex<double>::Float(3.0/2.0)};
  assert_parsed_expression_evaluates_to("int(x, 1, 2)", j);

  Complex<float> k[1] = {Complex<float>::Float(46098.0f)};
  assert_parsed_expression_evaluates_to("lcm(234,394)", k);

  Complex<double> l[1] = {Complex<double>::Float(std::log(2.0))};
  assert_parsed_expression_evaluates_to("ln(2)", l);

  Complex<float> m[1] = {Complex<float>::Float(std::log10(2.0f))};
  assert_parsed_expression_evaluates_to("log(2)", m);

  Complex<double> n[1] = {Complex<double>::Float(5040.0)};
  assert_parsed_expression_evaluates_to("permute(10, 4)", n);

  Complex<float> o[1] = {Complex<float>::Float(604800.0f)};
  assert_parsed_expression_evaluates_to("product(n, 4, 10)", o);

  Complex<double> p[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("re(2+I)", p);

  Complex<float> q[1] = {Complex<float>::Float(9.0f)};
  assert_parsed_expression_evaluates_to("rem(29, 10)", q);

  Complex<double> r[1] = {Complex<double>::Float(std::pow(2.0, 1.0/3.0))};
  assert_parsed_expression_evaluates_to("root(2,3)", r);

  Complex<float> s[1] = {Complex<float>::Float(std::sqrt(2.0f))};
  assert_parsed_expression_evaluates_to("R(2)", s);

  Complex<double> t[1] = {Complex<double>::Float(49.0)};
  assert_parsed_expression_evaluates_to("sum(n, 4, 10)", t);

#if MATRICES_ARE_DEFINED
  Complex<float> u[1] = {Complex<float>::Float(15.0f)};
  assert_parsed_expression_evaluates_to("trace([[1,2,3][4,5,6][7,8,9]])", u);
#endif

  Complex<double> v[2] = {Complex<double>::Float(0.1 - std::sqrt(1.0/100.0)), Complex<double>::Float(0.1 + std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluates_to("confidence(0.1, 100)", v, 2);

#if MATRICES_ARE_DEFINED
  Complex<float> w[2] = {Complex<float>::Float(2.0f), Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("dim([[1,2,3][4,5,-6]])", w, 2);
#endif

  Complex<double> x[1] = {Complex<double>::Cartesian(3.0, -2.0)};
  assert_parsed_expression_evaluates_to("conj(3+2*I)", x);

#if MATRICES_ARE_DEFINED
  Complex<float> y[9] = {Complex<float>::Float(-31.0f/24.0f), Complex<float>::Float(-1.0f/12.0f), Complex<float>::Float(3.0f/8.0f), Complex<float>::Float(13.0f/12.0f), Complex<float>::Float(1.0f/6.0f), Complex<float>::Float(-1.0f/4.0f), Complex<float>::Float(1.0f/24.0f),Complex<float>::Float(-1.0f/12.0f), Complex<float>::Float(1.0f/24.0f)};
  assert_parsed_expression_evaluates_to("inverse([[1,2,3][4,5,-6][7,8,9]])", y, 3, 3);
#endif

  Complex<double> z[2] = {Complex<double>::Float(0.1-std::sqrt(1.0/100.0)), Complex<double>::Float(0.1+std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluates_to("prediction(0.1, 100)", z, 2);

  Complex<float> aa[2] = {Complex<float>::Float(0.1f-1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f)), Complex<float>::Float(0.1f+1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f))};
  assert_parsed_expression_evaluates_to("prediction95(0.1, 100)", aa, 2);

  Complex<double> ab[1] = {Complex<double>::Cartesian(-100.0, -540.0)};
  assert_parsed_expression_evaluates_to("product(2+n*I, 1, 5)", ab);

  Complex<float> ac[1] = {Complex<float>::Cartesian(1.4593656008f, 0.1571201229f)};
  assert_parsed_expression_evaluates_to("root(3+I, 3)", ac);

  Complex<double> ad[1] = {Complex<double>::Cartesian(1.38200696233, -0.152442779)};
  assert_parsed_expression_evaluates_to("root(3, 3+I)", ad);

  Complex<float> ae[1] = {Complex<float>::Cartesian(1.75532f, 0.28485f)};
  assert_parsed_expression_evaluates_to("R(3+I)", ae);

  Complex<double> af[1] = {Complex<double>::Cartesian(10.0, 15.0)};
  assert_parsed_expression_evaluates_to("sum(2+n*I,1,5)", af);
#if MATRICES_ARE_DEFINED
  Complex<double> ag[9] = {Complex<double>::Float(1.0), Complex<double>::Float(4.0), Complex<double>::Float(7.0), Complex<double>::Float(2.0), Complex<double>::Float(5.0), Complex<double>::Float(8.0), Complex<double>::Float(3.0), Complex<double>::Float(-6.0), Complex<double>::Float(9.0)};
  assert_parsed_expression_evaluates_to("transpose([[1,2,3][4,5,-6][7,8,9]])", ag, 3, 3);
  assert_parsed_expression_evaluates_to("transpose([[1,7,5][4,2,8]])", ag, 3, 2);
  assert_parsed_expression_evaluates_to("transpose([[1,2][4,5][7,8]])", ag, 2, 3);
#endif

  Complex<float> ah[1] = {Complex<float>::Float(2.325f)};
  assert_parsed_expression_evaluates_to("round(2.3245,3)", ah);

  Complex<double> ai[1] = {Complex<double>::Float(720.0f)};
  assert_parsed_expression_evaluates_to("6!", ai);

  Complex<float> aj[1] = {Complex<float>::Cartesian(0.0f, 1.0f)};
  assert_parsed_expression_evaluates_to("R(-1)", aj);

  Complex<double> ak[1] = {Complex<double>::Cartesian(0.5, 0.86602540378443864676)};
  assert_parsed_expression_evaluates_to("root(-1,3)", ak);

  Complex<double> al[1] = {Complex<double>::Float(0)};
  assert_parsed_expression_evaluates_to("arg(0)", al);
}
