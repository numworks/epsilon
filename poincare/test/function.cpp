#include <quiz.h>
#include <poincare.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_function) {
  Expression * e = Expression::parse("abs(-1)");
  assert(e->type() == Expression::Type::AbsoluteValue);
  delete e;

  char argText[10] = {'a', 'r', 'g', '(', '2', '+', Ion::Charset::IComplex, ')', 0};
  e = Expression::parse(argText);
  assert(e->type() == Expression::Type::ComplexArgument);
  delete e;

  e = Expression::parse("binomial(10, 4)");
  assert(e->type() == Expression::Type::BinomialCoefficient);
  delete e;

  e = Expression::parse("ceil(0.2)");
  assert(e->type() == Expression::Type::Ceiling);
  delete e;

  e = Expression::parse("diff(2*x, 2)");
  assert(e->type() == Expression::Type::Derivative);
  delete e;

#if MATRICES_ARE_DEFINED
  e = Expression::parse("dim([[2]])");
  assert(e->type() == Expression::Type::MatrixDimension);
  delete e;

  e = Expression::parse("det([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::Determinant);
  delete e;
#endif

  e = Expression::parse("confidence(0.1, 100)");
  assert(e->type() == Expression::Type::ConfidenceInterval);
  delete e;

  e = Expression::parse("conj(2)");
  assert(e->type() == Expression::Type::Conjugate);
  delete e;

  e = Expression::parse("floor(2.3)");
  assert(e->type() == Expression::Type::Floor);
  delete e;

  e = Expression::parse("frac(2.3)");
  assert(e->type() == Expression::Type::FracPart);
  delete e;

  e = Expression::parse("gcd(2,3)");
  assert(e->type() == Expression::Type::GreatCommonDivisor);
  delete e;

  char imText[10] = {'i', 'm', '(', '2', '+', Ion::Charset::IComplex, ')', 0};
  e = Expression::parse(imText);
  assert(e->type() == Expression::Type::ImaginaryPart);
  delete e;

  e = Expression::parse("int(x, 2, 3)");
  assert(e->type() == Expression::Type::Integral);
  delete e;

#if MATRICES_ARE_DEFINED
  e = Expression::parse("inverse([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixInverse);
  delete e;
#endif

  e = Expression::parse("lcm(2,3)");
  assert(e->type() == Expression::Type::LeastCommonMultiple);
  delete e;

  e = Expression::parse("ln(2)");
  assert(e->type() == Expression::Type::NaperianLogarithm);
  delete e;

  e = Expression::parse("log(2)");
  assert(e->type() == Expression::Type::Logarithm);
  delete e;

  e = Expression::parse("permute(10, 4)");
  assert(e->type() == Expression::Type::PermuteCoefficient);
  delete e;

  e = Expression::parse("prediction(0.1, 100)");
  assert(e->type() == Expression::Type::ConfidenceInterval);
  delete e;

  e = Expression::parse("prediction95(0.1, 100)");
  assert(e->type() == Expression::Type::PredictionInterval);
  delete e;

  e = Expression::parse("product(n, 4, 10)");
  assert(e->type() == Expression::Type::Product);
  delete e;

  e = Expression::parse("quo(29, 10)");
  assert(e->type() == Expression::Type::DivisionQuotient);
  delete e;

  char reText[10] = {'r', 'e', '(', '2', '+', Ion::Charset::IComplex, ')', 0};
  e = Expression::parse(reText);
  assert(e->type() == Expression::Type::ReelPart);
  delete e;

  e = Expression::parse("rem(29, 10)");
  assert(e->type() == Expression::Type::DivisionRemainder);
  delete e;

  e = Expression::parse("root(2,3)");
  assert(e->type() == Expression::Type::NthRoot);
  delete e;

  char text[5] = {Ion::Charset::Root, '(', '2', ')', 0};
  e = Expression::parse(text);
  assert(e->type() == Expression::Type::SquareRoot);
  delete e;

  e = Expression::parse("round(2,3)");
  assert(e->type() == Expression::Type::Round);
  delete e;

  e = Expression::parse("sum(n, 4, 10)");
  assert(e->type() == Expression::Type::Sum);
  delete e;

#if MATRICES_ARE_DEFINED
  e = Expression::parse("trace([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixTrace);
  delete e;

  e = Expression::parse("transpose([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixTranspose);
  delete e;
#endif

  e = Expression::parse("6!");
  assert(e->type() == Expression::Type::Factorial);
  delete e;
}


QUIZ_CASE(poincare_function_evaluate) {
  Complex<double> a0[1] = {Complex<double>::Float(1.0)};
  assert_parsed_expression_evaluate_to("abs(-1)", a0);

  Complex<float> a1[1] = {Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluate_to("abs(3+2I)", a1);

  Complex<double> a2[4] = {Complex<double>::Float(1.0), Complex<double>::Float(2.0), Complex<double>::Float(3.0), Complex<double>::Float(4.0)};
  assert_parsed_expression_evaluate_to("abs([[1,-2][3,-4]])", a2, 2, 2);

  Complex<float> a3[4] = {Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f)), Complex<float>::Float(std::sqrt(3.0f*3.0f+4.0f*4.0f)), Complex<float>::Float(std::sqrt(5.0f*5.0f+2.0f*2.0f)), Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluate_to("abs([[3+2I,3+4I][5+2I,3+2I]])", a3, 2, 2);

  Complex<double> b[1] = {Complex<double>::Float(210.0)};
  assert_parsed_expression_evaluate_to("binomial(10, 4)", b);

  Complex<float> c[1] = {Complex<float>::Float(1.0f)};
  assert_parsed_expression_evaluate_to("ceil(0.2)", c);

  Complex<double> d[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluate_to("diff(2*x, 2)", d);

#if MATRICES_ARE_DEFINED
  Complex<float> e[1] = {Complex<float>::Float(126.0f)};
  assert_parsed_expression_evaluate_to("det([[1,23,3][4,5,6][7,8,9]])", e);
#endif

  Complex<double> f[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluate_to("floor(2.3)", f);

  Complex<float> g[1] = {Complex<float>::Float(0.3f)};
  assert_parsed_expression_evaluate_to("frac(2.3)", g);

  Complex<double> h[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluate_to("gcd(234,394)", h);

  Complex<float> i[1] = {Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluate_to("im(2+3I)", i);

  Complex<double> j[1] = {Complex<double>::Float(3.0/2.0)};
  assert_parsed_expression_evaluate_to("int(x, 1, 2)", j);

  Complex<float> k[1] = {Complex<float>::Float(46098.0f)};
  assert_parsed_expression_evaluate_to("lcm(234,394)", k);

  Complex<double> l[1] = {Complex<double>::Float(std::log(2.0))};
  assert_parsed_expression_evaluate_to("ln(2)", l);

  Complex<float> m[1] = {Complex<float>::Float(std::log10(2.0f))};
  assert_parsed_expression_evaluate_to("log(2)", m);

  Complex<double> n[1] = {Complex<double>::Float(5040.0)};
  assert_parsed_expression_evaluate_to("permute(10, 4)", n);

  Complex<float> o[1] = {Complex<float>::Float(604800.0f)};
  assert_parsed_expression_evaluate_to("product(n, 4, 10)", o);

  Complex<double> p[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluate_to("re(2+I)", p);

  Complex<float> q[1] = {Complex<float>::Float(9.0f)};
  assert_parsed_expression_evaluate_to("rem(29, 10)", q);

  Complex<double> r[1] = {Complex<double>::Float(std::pow(2.0, 1.0/3.0))};
  assert_parsed_expression_evaluate_to("root(2,3)", r);

  Complex<float> s[1] = {Complex<float>::Float(std::sqrt(2.0f))};
  assert_parsed_expression_evaluate_to("R(2)", s);

  Complex<double> t[1] = {Complex<double>::Float(49.0)};
  assert_parsed_expression_evaluate_to("sum(n, 4, 10)", t);

#if MATRICES_ARE_DEFINED
  Complex<float> u[1] = {Complex<float>::Float(15.0f)};
  assert_parsed_expression_evaluate_to("trace([[1,2,3][4,5,6][7,8,9]])", u);
#endif

  Complex<double> v[2] = {Complex<double>::Float(0.1 - std::sqrt(1.0/100.0)), Complex<double>::Float(0.1 + std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluate_to("confidence(0.1, 100)", v, 2);

#if MATRICES_ARE_DEFINED
  Complex<float> w[2] = {Complex<float>::Float(2.0f), Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluate_to("dim([[1,2,3][4,5,-6]])", w, 2);
#endif

  Complex<double> x[1] = {Complex<double>::Cartesian(3.0, -2.0)};
  assert_parsed_expression_evaluate_to("conj(3+2*I)", x);

#if MATRICES_ARE_DEFINED
  Complex<float> y[9] = {Complex<float>::Float(-31.0f/24.0f), Complex<float>::Float(-1.0f/12.0f), Complex<float>::Float(3.0f/8.0f), Complex<float>::Float(13.0f/12.0f), Complex<float>::Float(1.0f/6.0f), Complex<float>::Float(-1.0f/4.0f), Complex<float>::Float(1.0f/24.0f),Complex<float>::Float(-1.0f/12.0f), Complex<float>::Float(1.0f/24.0f)};
  assert_parsed_expression_evaluate_to("inverse([[1,2,3][4,5,-6][7,8,9]])", y, 3, 3);
#endif

  Complex<double> z[2] = {Complex<double>::Float(0.1-std::sqrt(1.0/100.0)), Complex<double>::Float(0.1+std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluate_to("prediction(0.1, 100)", z, 2);

  Complex<float> aa[2] = {Complex<float>::Float(0.1f-1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f)), Complex<float>::Float(0.1f+1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f))};
  assert_parsed_expression_evaluate_to("prediction95(0.1, 100)", aa, 2);

  Complex<double> ab[1] = {Complex<double>::Cartesian(-100.0, -540.0)};
  assert_parsed_expression_evaluate_to("product(2+n*I, 1, 5)", ab);

  Complex<float> ac[1] = {Complex<float>::Cartesian(1.4593656008f, 0.1571201229f)};
  assert_parsed_expression_evaluate_to("root(3+I, 3)", ac);

  Complex<double> ad[1] = {Complex<double>::Cartesian(1.38200696233, -0.152442779)};
  assert_parsed_expression_evaluate_to("root(3, 3+I)", ad);

  Complex<float> ae[1] = {Complex<float>::Cartesian(1.75532f, 0.28485f)};
  assert_parsed_expression_evaluate_to("R(3+I)", ae);

  Complex<double> af[1] = {Complex<double>::Cartesian(10.0, 15.0)};
  assert_parsed_expression_evaluate_to("sum(2+n*I,1,5)", af);
#if MATRICES_ARE_DEFINED
  Complex<double> ag[9] = {Complex<double>::Float(1.0), Complex<double>::Float(4.0), Complex<double>::Float(7.0), Complex<double>::Float(2.0), Complex<double>::Float(5.0), Complex<double>::Float(8.0), Complex<double>::Float(3.0), Complex<double>::Float(-6.0), Complex<double>::Float(9.0)};
  assert_parsed_expression_evaluate_to("transpose([[1,2,3][4,5,-6][7,8,9]])", ag, 3, 3);
  assert_parsed_expression_evaluate_to("transpose([[1,7,5][4,2,8]])", ag, 3, 2);
  assert_parsed_expression_evaluate_to("transpose([[1,2][4,5][7,8]])", ag, 2, 3);
#endif

  Complex<float> ah[1] = {Complex<float>::Float(2.325f)};
  assert_parsed_expression_evaluate_to("round(2.3245,3)", ah);

  Complex<double> ai[1] = {Complex<double>::Float(720.0f)};
  assert_parsed_expression_evaluate_to("6!", ai);
}
