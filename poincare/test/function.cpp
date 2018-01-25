#include <quiz.h>
#include <poincare.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_exp_is_bounded(Expression * exp, T lowBound, T upBound) {
  GlobalContext globalContext;
  Expression * result = exp->approximate<T>(globalContext);
  assert(result->type() == Expression::Type::Complex);
  assert(static_cast<const Complex<T> *>(result)->a() < upBound && static_cast<const Complex<T> *>(result)->a() >= lowBound);
  delete result;
}

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
  assert_parsed_expression_type("factor(23/42)", Expression::Type::Factor);
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
  assert_parsed_expression_type("random()", Expression::Type::Random);
  assert_parsed_expression_type("randint(1, 2)", Expression::Type::Randint);
  assert_parsed_expression_type("re(2+I)", Expression::Type::RealPart);
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
  Complex<float> a0[1] = {Complex<float>::Float(1.0)};
  assert_parsed_expression_evaluates_to("abs(-1)", a0);
  Complex<double> ad0[1] = {Complex<double>::Float(1.0)};
  assert_parsed_expression_evaluates_to("abs(-1)", ad0);

  Complex<float> a1[1] = {Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluates_to("abs(3+2I)", a1);
  Complex<double> ad1[1] = {Complex<double>::Float(std::sqrt(3.0*3.0+2.0*2.0))};
  assert_parsed_expression_evaluates_to("abs(3+2I)", ad1);

  Complex<float> a2[4] = {Complex<float>::Float(1.0), Complex<float>::Float(2.0), Complex<float>::Float(3.0), Complex<float>::Float(4.0)};
  assert_parsed_expression_evaluates_to("abs([[1,-2][3,-4]])", a2, 2, 2);
  Complex<double> ad2[4] = {Complex<double>::Float(1.0), Complex<double>::Float(2.0), Complex<double>::Float(3.0), Complex<double>::Float(4.0)};
  assert_parsed_expression_evaluates_to("abs([[1,-2][3,-4]])", ad2, 2, 2);

  Complex<float> a3[4] = {Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f)), Complex<float>::Float(std::sqrt(3.0f*3.0f+4.0f*4.0f)), Complex<float>::Float(std::sqrt(5.0f*5.0f+2.0f*2.0f)), Complex<float>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluates_to("abs([[3+2I,3+4I][5+2I,3+2I]])", a3, 2, 2);
  Complex<double> ad3[4] = {Complex<double>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f)), Complex<double>::Float(std::sqrt(3.0f*3.0f+4.0f*4.0f)), Complex<double>::Float(std::sqrt(5.0f*5.0f+2.0f*2.0f)), Complex<double>::Float(std::sqrt(3.0f*3.0f+2.0f*2.0f))};
  assert_parsed_expression_evaluates_to("abs([[3+2I,3+4I][5+2I,3+2I]])", ad3, 2, 2);

  Complex<float> b[1] = {Complex<float>::Float(210.0)};
  assert_parsed_expression_evaluates_to("binomial(10, 4)", b);
  Complex<double> bd[1] = {Complex<double>::Float(210.0)};
  assert_parsed_expression_evaluates_to("binomial(10, 4)", bd);

  Complex<float> c[1] = {Complex<float>::Float(1.0f)};
  assert_parsed_expression_evaluates_to("ceil(0.2)", c);
  Complex<double> cd[1] = {Complex<double>::Float(1.0f)};
  assert_parsed_expression_evaluates_to("ceil(0.2)", cd);

  Complex<float> d[1] = {Complex<float>::Float(2.0)};
  assert_parsed_expression_evaluates_to("diff(2*x, 2)", d);
  Complex<double> dd[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("diff(2*x, 2)", dd);

#if MATRICES_ARE_DEFINED
  Complex<float> e[1] = {Complex<float>::Float(126.0f)};
  assert_parsed_expression_evaluates_to("det([[1,23,3][4,5,6][7,8,9]])", e);
  Complex<double> ed[1] = {Complex<double>::Float(126.0f)};
  assert_parsed_expression_evaluates_to("det([[1,23,3][4,5,6][7,8,9]])", ed);
#endif

  Complex<float> f[1] = {Complex<float>::Float(2.0)};
  assert_parsed_expression_evaluates_to("floor(2.3)", f);
  Complex<double> fd[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("floor(2.3)", fd);

  Complex<float> g[1] = {Complex<float>::Float(0.3f)};
  assert_parsed_expression_evaluates_to("frac(2.3)", g);
  Complex<double> gd[1] = {Complex<double>::Float(0.3f)};
  assert_parsed_expression_evaluates_to("frac(2.3)", gd);

  Complex<float> h[1] = {Complex<float>::Float(2.0)};
  assert_parsed_expression_evaluates_to("gcd(234,394)", h);
  Complex<double> hd[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("gcd(234,394)", hd);

  Complex<float> i[1] = {Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("im(2+3I)", i);
  Complex<double> id[1] = {Complex<double>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("im(2+3I)", id);

  Complex<float> j[1] = {Complex<float>::Float(3.0/2.0)};
  assert_parsed_expression_evaluates_to("int(x, 1, 2)", j);
  Complex<double> jd[1] = {Complex<double>::Float(3.0/2.0)};
  assert_parsed_expression_evaluates_to("int(x, 1, 2)", jd);

  Complex<float> k[1] = {Complex<float>::Float(46098.0f)};
  assert_parsed_expression_evaluates_to("lcm(234,394)", k);
  Complex<double> kd[1] = {Complex<double>::Float(46098.0f)};
  assert_parsed_expression_evaluates_to("lcm(234,394)", kd);

  Complex<float> l[1] = {Complex<float>::Float(std::log(2.0))};
  assert_parsed_expression_evaluates_to("ln(2)", l);
  Complex<double> ld[1] = {Complex<double>::Float(std::log(2.0))};
  assert_parsed_expression_evaluates_to("ln(2)", ld);

  Complex<float> m[1] = {Complex<float>::Float(std::log10(2.0f))};
  assert_parsed_expression_evaluates_to("log(2)", m);
  Complex<double> md[1] = {Complex<double>::Float(std::log10(2.0f))};
  assert_parsed_expression_evaluates_to("log(2)", md);

  Complex<float> n[1] = {Complex<float>::Float(5040.0)};
  assert_parsed_expression_evaluates_to("permute(10, 4)", n);
  Complex<double> nd[1] = {Complex<double>::Float(5040.0)};
  assert_parsed_expression_evaluates_to("permute(10, 4)", nd);

  Complex<float> o[1] = {Complex<float>::Float(604800.0f)};
  assert_parsed_expression_evaluates_to("product(n, 4, 10)", o);
  Complex<double> od[1] = {Complex<double>::Float(604800.0f)};
  assert_parsed_expression_evaluates_to("product(n, 4, 10)", od);

  Complex<float> p[1] = {Complex<float>::Float(2.0)};
  assert_parsed_expression_evaluates_to("re(2+I)", p);
  Complex<double> pd[1] = {Complex<double>::Float(2.0)};
  assert_parsed_expression_evaluates_to("re(2+I)", pd);

  Complex<float> q[1] = {Complex<float>::Float(9.0f)};
  assert_parsed_expression_evaluates_to("rem(29, 10)", q);
  Complex<double> qd[1] = {Complex<double>::Float(9.0f)};
  assert_parsed_expression_evaluates_to("rem(29, 10)", qd);

  Complex<float> r[1] = {Complex<float>::Float(std::pow(2.0, 1.0/3.0))};
  assert_parsed_expression_evaluates_to("root(2,3)", r);
  Complex<double> rd[1] = {Complex<double>::Float(std::pow(2.0, 1.0/3.0))};
  assert_parsed_expression_evaluates_to("root(2,3)", rd);

  Complex<float> s[1] = {Complex<float>::Float(std::sqrt(2.0f))};
  assert_parsed_expression_evaluates_to("R(2)", s);
  Complex<double> sd[1] = {Complex<double>::Float(std::sqrt(2.0f))};
  assert_parsed_expression_evaluates_to("R(2)", sd);

  Complex<float> t[1] = {Complex<float>::Float(49.0)};
  assert_parsed_expression_evaluates_to("sum(n, 4, 10)", t);
  Complex<double> td[1] = {Complex<double>::Float(49.0)};
  assert_parsed_expression_evaluates_to("sum(n, 4, 10)", td);

#if MATRICES_ARE_DEFINED
  Complex<float> u[1] = {Complex<float>::Float(15.0f)};
  assert_parsed_expression_evaluates_to("trace([[1,2,3][4,5,6][7,8,9]])", u);
  Complex<double> ud[1] = {Complex<double>::Float(15.0f)};
  assert_parsed_expression_evaluates_to("trace([[1,2,3][4,5,6][7,8,9]])", ud);
#endif

  Complex<float> v[2] = {Complex<float>::Float(0.1 - std::sqrt(1.0/100.0)), Complex<float>::Float(0.1 + std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluates_to("confidence(0.1, 100)", v, 1, 2);
  Complex<double> vd[2] = {Complex<double>::Float(0.1 - std::sqrt(1.0/100.0)), Complex<double>::Float(0.1 + std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluates_to("confidence(0.1, 100)", vd, 1, 2);

#if MATRICES_ARE_DEFINED
  Complex<float> w[2] = {Complex<float>::Float(2.0f), Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("dim([[1,2,3][4,5,-6]])", w, 1, 2);
  Complex<double> wd[2] = {Complex<double>::Float(2.0f), Complex<double>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("dim([[1,2,3][4,5,-6]])", wd, 1, 2);
#endif

  Complex<float> x[1] = {Complex<float>::Cartesian(3.0, -2.0)};
  assert_parsed_expression_evaluates_to("conj(3+2*I)", x);
  Complex<double> xd[1] = {Complex<double>::Cartesian(3.0, -2.0)};
  assert_parsed_expression_evaluates_to("conj(3+2*I)", xd);

#if MATRICES_ARE_DEFINED
  Complex<float> y[9] = {Complex<float>::Float(-31.0f/24.0f), Complex<float>::Float(-1.0f/12.0f), Complex<float>::Float(3.0f/8.0f), Complex<float>::Float(13.0f/12.0f), Complex<float>::Float(1.0f/6.0f), Complex<float>::Float(-1.0f/4.0f), Complex<float>::Float(1.0f/24.0f),Complex<float>::Float(-1.0f/12.0f), Complex<float>::Float(1.0f/24.0f)};
  assert_parsed_expression_evaluates_to("inverse([[1,2,3][4,5,-6][7,8,9]])", y, 3, 3);
  Complex<double> yd[9] = {Complex<double>::Float(-31.0f/24.0f), Complex<double>::Float(-1.0f/12.0f), Complex<double>::Float(3.0f/8.0f), Complex<double>::Float(13.0f/12.0f), Complex<double>::Float(1.0f/6.0f), Complex<double>::Float(-1.0f/4.0f), Complex<double>::Float(1.0f/24.0f),Complex<double>::Float(-1.0f/12.0f), Complex<double>::Float(1.0f/24.0f)};
  assert_parsed_expression_evaluates_to("inverse([[1,2,3][4,5,-6][7,8,9]])", yd, 3, 3);
#endif

  Complex<float> z[2] = {Complex<float>::Float(0.1-std::sqrt(1.0/100.0)), Complex<float>::Float(0.1+std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluates_to("prediction(0.1, 100)", z, 1, 2);
  Complex<double> zd[2] = {Complex<double>::Float(0.1-std::sqrt(1.0/100.0)), Complex<double>::Float(0.1+std::sqrt(1.0/100.0))};
  assert_parsed_expression_evaluates_to("prediction(0.1, 100)", zd, 1, 2);

  Complex<float> aa[2] = {Complex<float>::Float(0.1f-1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f)), Complex<float>::Float(0.1f+1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f))};
  assert_parsed_expression_evaluates_to("prediction95(0.1, 100)", aa, 1, 2);
  Complex<double> aad[2] = {Complex<double>::Float(0.1f-1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f)), Complex<double>::Float(0.1f+1.96f*std::sqrt((0.1f*(1.0f-0.1f))/100.0f))};
  assert_parsed_expression_evaluates_to("prediction95(0.1, 100)", aad, 1, 2);

  Complex<float> ab[1] = {Complex<float>::Cartesian(-100.0, -540.0)};
  assert_parsed_expression_evaluates_to("product(2+n*I, 1, 5)", ab);
  Complex<double> abd[1] = {Complex<double>::Cartesian(-100.0, -540.0)};
  assert_parsed_expression_evaluates_to("product(2+n*I, 1, 5)", abd);

  Complex<float> ac[1] = {Complex<float>::Cartesian(1.4593656008f, 0.1571201229f)};
  assert_parsed_expression_evaluates_to("root(3+I, 3)", ac);
  Complex<double> acd[1] = {Complex<double>::Cartesian(1.4593656008f, 0.1571201229f)};
  assert_parsed_expression_evaluates_to("root(3+I, 3)", acd);

  Complex<float> add[1] = {Complex<float>::Cartesian(1.38200696233, -0.152442779)};
  assert_parsed_expression_evaluates_to("root(3, 3+I)", add);
  Complex<double> addd[1] = {Complex<double>::Cartesian(1.38200696233, -0.152442779)};
  assert_parsed_expression_evaluates_to("root(3, 3+I)", addd);

  Complex<float> ae[1] = {Complex<float>::Cartesian(1.75532f, 0.28485f)};
  assert_parsed_expression_evaluates_to("R(3+I)", ae);
  Complex<double> aed[1] = {Complex<double>::Cartesian(1.75532f, 0.28485f)};
  assert_parsed_expression_evaluates_to("R(3+I)", aed);

  Complex<float> af[1] = {Complex<float>::Cartesian(10.0, 15.0)};
  assert_parsed_expression_evaluates_to("sum(2+n*I,1,5)", af);
  Complex<double> afd[1] = {Complex<double>::Cartesian(10.0, 15.0)};
  assert_parsed_expression_evaluates_to("sum(2+n*I,1,5)", afd);
#if MATRICES_ARE_DEFINED
  Complex<float> ag[9] = {Complex<float>::Float(1.0), Complex<float>::Float(4.0), Complex<float>::Float(7.0), Complex<float>::Float(2.0), Complex<float>::Float(5.0), Complex<float>::Float(8.0), Complex<float>::Float(3.0), Complex<float>::Float(-6.0), Complex<float>::Float(9.0)};
  assert_parsed_expression_evaluates_to("transpose([[1,2,3][4,5,-6][7,8,9]])", ag, 3, 3);
  assert_parsed_expression_evaluates_to("transpose([[1,7,5][4,2,8]])", ag, 3, 2);
  assert_parsed_expression_evaluates_to("transpose([[1,2][4,5][7,8]])", ag, 2, 3);
  Complex<double> agd[9] = {Complex<double>::Float(1.0), Complex<double>::Float(4.0), Complex<double>::Float(7.0), Complex<double>::Float(2.0), Complex<double>::Float(5.0), Complex<double>::Float(8.0), Complex<double>::Float(3.0), Complex<double>::Float(-6.0), Complex<double>::Float(9.0)};
  assert_parsed_expression_evaluates_to("transpose([[1,2,3][4,5,-6][7,8,9]])", agd, 3, 3);
  assert_parsed_expression_evaluates_to("transpose([[1,7,5][4,2,8]])", agd, 3, 2);
  assert_parsed_expression_evaluates_to("transpose([[1,2][4,5][7,8]])", agd, 2, 3);
#endif

  Complex<float> ah[1] = {Complex<float>::Float(2.325f)};
  assert_parsed_expression_evaluates_to("round(2.3246,3)", ah);
  Complex<double> ahd[1] = {Complex<double>::Float(2.325f)};
  assert_parsed_expression_evaluates_to("round(2.3245,3)", ahd);

  Complex<float> ai[1] = {Complex<float>::Float(720.0f)};
  assert_parsed_expression_evaluates_to("6!", ai);
  Complex<double> aid[1] = {Complex<double>::Float(720.0f)};
  assert_parsed_expression_evaluates_to("6!", aid);

  Complex<float> aj[1] = {Complex<float>::Cartesian(0.0f, 1.0f)};
  assert_parsed_expression_evaluates_to("R(-1)", aj);
  Complex<double> ajd[1] = {Complex<double>::Cartesian(0.0f, 1.0f)};
  assert_parsed_expression_evaluates_to("R(-1)", ajd);

  Complex<float> ak[1] = {Complex<float>::Cartesian(0.5, 0.86602540378443864676)};
  assert_parsed_expression_evaluates_to("root(-1,3)", ak);
  Complex<double> akd[1] = {Complex<double>::Cartesian(0.5, 0.86602540378443864676)};
  assert_parsed_expression_evaluates_to("root(-1,3)", akd);

  Complex<float> al[1] = {Complex<float>::Float(-5.75f)};
  assert_parsed_expression_evaluates_to("factor(-23/4)", al);
  Complex<double> ald[1] = {Complex<double>::Float(-5.125)};
  assert_parsed_expression_evaluates_to("factor(-123/24)", ald);

  Expression * exp = parse_expression("random()");
  assert_exp_is_bounded(exp, 0.0f, 1.0f);
  assert_exp_is_bounded(exp, 0.0, 1.0);
  delete exp;

  exp = parse_expression("randint(4,45)");
  assert_exp_is_bounded(exp, 4.0f, 45.0f);
  assert_exp_is_bounded(exp, 4.0, 45.0);
  delete exp;
}

QUIZ_CASE(poincare_function_simplify) {
  assert_parsed_expression_simplify_to("abs(P)", "P");
  assert_parsed_expression_simplify_to("abs(-P)", "P");
  assert_parsed_expression_simplify_to("binomial(20,3)", "1140");
  assert_parsed_expression_simplify_to("binomial(20,10)", "184756");
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1");
  assert_parsed_expression_simplify_to("conj(1/2)", "1/2");
  assert_parsed_expression_simplify_to("quo(19,3)", "6");
  assert_parsed_expression_simplify_to("quo(19,0)", "undef");
  assert_parsed_expression_simplify_to("quo(-19,3)", "-7");
  assert_parsed_expression_simplify_to("rem(19,3)", "1");
  assert_parsed_expression_simplify_to("rem(-19,3)", "2");
  assert_parsed_expression_simplify_to("rem(19,0)", "undef");
  assert_parsed_expression_simplify_to("99!", "933262154439441526816992388562667004907159682643816214685929638952175999932299156089414639761565182862536979208272237582511852109168640000000000000000000000");
  assert_parsed_expression_simplify_to("factor(-10008/6895)", "-(2^3*3^2*139)/(5*7*197)");
  assert_parsed_expression_simplify_to("factor(1008/6895)", "(2^4*3^2)/(5*197)");
  assert_parsed_expression_simplify_to("factor(10007)", "10007");
  assert_parsed_expression_simplify_to("factor(10007^2)", "undef");
  assert_parsed_expression_simplify_to("floor(-1.3)", "-2");
  assert_parsed_expression_simplify_to("frac(-1.3)", "7/10");
  assert_parsed_expression_simplify_to("gcd(123,278)", "1");
  assert_parsed_expression_simplify_to("gcd(11,121)", "11");
  assert_parsed_expression_simplify_to("lcm(123,278)", "34194");
  assert_parsed_expression_simplify_to("lcm(11,121)", "121");
  assert_parsed_expression_simplify_to("R(4)", "2");
  assert_parsed_expression_simplify_to("root(4,3)", "root(4,3)");
  assert_parsed_expression_simplify_to("root(4,P)", "4^(1/P)");
  assert_parsed_expression_simplify_to("root(27,3)", "3");
  assert_parsed_expression_simplify_to("round(4.235,2)", "106/25");
  assert_parsed_expression_simplify_to("round(4.23,0)", "4");
  assert_parsed_expression_simplify_to("round(4.9,0)", "5");
  assert_parsed_expression_simplify_to("round(12.9,-1)", "10");
  assert_parsed_expression_simplify_to("round(12.9,-2)", "0");
  assert_parsed_expression_simplify_to("permute(99,4)", "90345024");
  assert_parsed_expression_simplify_to("permute(20,-10)", "undef");
  assert_parsed_expression_simplify_to("re(1/2)", "1/2");
}
