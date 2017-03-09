#include <quiz.h>
#include <poincare.h>
#include <math.h>
#include <ion.h>
#include <assert.h>

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

  e = Expression::parse("dim([[2]])");
  assert(e->type() == Expression::Type::MatrixDimension);
  delete e;

  e = Expression::parse("det([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::Determinant);
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

  e = Expression::parse("inverse([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixInverse);
  delete e;

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

  e = Expression::parse("trace([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixTrace);
  delete e;

  e = Expression::parse("transpose([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixTranspose);
  delete e;
}


QUIZ_CASE(poincare_function_approximate) {
  GlobalContext globalContext;
  Expression * e = Expression::parse("abs(-1)");
  assert(e->approximate(globalContext) == 1.0f);
  delete e;

  char argText[10] = {'a', 'r', 'g', '(', '2', '+', Ion::Charset::IComplex, ')', 0};
  e = Expression::parse(argText);
  assert(fabsf(e->approximate(globalContext)-atanf(0.5f)) <= 0.000001f);
  delete e;

  e = Expression::parse("binomial(10, 4)");
  assert(e->approximate(globalContext) == 210.0f);
  delete e;

  e = Expression::parse("ceil(0.2)");
  assert(e->approximate(globalContext) == 1.0f);
  delete e;

  e = Expression::parse("diff(2*x, 2)");
  assert(e->approximate(globalContext) == 2.0f);
  delete e;

  e = Expression::parse("det([[1,23,3][4,5,6][7,8,9]])");
  assert(fabsf(e->approximate(globalContext)-126.0f) <= 0.0001f);
  delete e;

  e = Expression::parse("floor(2.3)");
  assert(e->approximate(globalContext) == 2.0f);
  delete e;

  e = Expression::parse("frac(2.3)");
  assert(fabsf(e->approximate(globalContext)-0.3f) <= 0.000001f);
  delete e;

  e = Expression::parse("gcd(234,394)");
  assert(e->approximate(globalContext) == 2.0f);
  delete e;

  char imText[10] = {'i', 'm', '(', '2', '+', '3','*', Ion::Charset::IComplex, ')', 0};
  e = Expression::parse(imText);
  assert(e->approximate(globalContext) == 3.0f);
  delete e;

  e = Expression::parse("int(x, 1, 2)");
  assert(e->approximate(globalContext) == 3.0f/2.0f);
  delete e;

  e = Expression::parse("lcm(234,394)");
  assert(e->approximate(globalContext) == 46098.0f);
  delete e;

  e = Expression::parse("ln(2)");
  assert(e->approximate(globalContext) == logf(2.0f));
  delete e;

  e = Expression::parse("log(2)");
  assert(e->approximate(globalContext) == log10f(2.0f));
  delete e;

  e = Expression::parse("permute(10, 4)");
  assert(e->approximate(globalContext) == 5040.0f);
  delete e;

  e = Expression::parse("product(n, 4, 10)");
  assert(e->approximate(globalContext) == 604800.0f);
  delete e;

  e = Expression::parse("quo(29, 10)");
  assert(e->approximate(globalContext) == 2.0f);
  delete e;

  char reText[10] = {'r', 'e', '(', '2', '+', Ion::Charset::IComplex, ')', 0};
  e = Expression::parse(reText);
  assert(e->approximate(globalContext) == 2.0f);
  delete e;

  e = Expression::parse("rem(29, 10)");
  assert(e->approximate(globalContext) == 9.0f);
  delete e;

  e = Expression::parse("root(2,3)");
  assert(e->approximate(globalContext) == powf(2.0f, 1.0f/3.0f));
  delete e;

  char text[5] = {Ion::Charset::Root, '(', '2', ')', 0};
  e = Expression::parse(text);
  assert(e->approximate(globalContext) == sqrtf(2.0f));
  delete e;

  e = Expression::parse("round(2.3245,3)");
  assert(e->approximate(globalContext) == 2.325f);
  delete e;

  e = Expression::parse("sum(n, 4, 10)");
  assert(e->approximate(globalContext) == 49.0f);
  delete e;

  e = Expression::parse("trace([[1,2,3][4,5,6][7,8,9]])");
  assert(e->approximate(globalContext) == 15.0f);
  delete e;
}

QUIZ_CASE(poincare_function_evaluate) {
  GlobalContext globalContext;
  char text[11] = {'a','b','s','(','3','+','2','*',Ion::Charset::IComplex,')', 0};
  Expression * a = Expression::parse(text);
  Expression * e = a->evaluate(globalContext);
  assert(((Complex *)e)->a() == sqrtf(3.0f*3.0f+2.0f*2.0f) &&
         ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  a = Expression::parse("dim([[1,2,3][4,5,-6]])");
  e = a->evaluate(globalContext);
  assert(fabsf(e->operand(0)->approximate(globalContext)-2.0f) <= 0.00001f);
  assert(fabsf(e->operand(1)->approximate(globalContext) -3.0f) <= 0.00001f);
  delete a;
  delete e;

  char conjText[20] ={'c','o', 'n','j', '(', '3', '+', '2', '*', Ion::Charset::IComplex, ')', 0};
  a = Expression::parse(conjText);
  e = a->evaluate(globalContext);
  assert(fabsf(3.0f - ((Complex *)e)->a()) <= 0.00001f &&
         fabsf(-2.0f - ((Complex *)e)->b()) <= 0.00001f);
  delete a;
  delete e;

  a = Expression::parse("inverse([[1,2,3][4,5,-6][7,8,9]])");
  e = a->evaluate(globalContext);
  assert(fabsf(e->operand(0)->approximate(globalContext)-(-31.0f/24.0f)) <= 0.00001f);
  assert(fabsf(e->operand(1)->approximate(globalContext) -(-1.0f/12.0f)) <= 0.00001f);
  assert(fabsf(e->operand(2)->approximate(globalContext) -(3.0f/8.0f)) <= 0.00001f);
  assert(fabsf(e->operand(3)->approximate(globalContext) -(13.0f/12.0f)) <= 0.00001f);
  assert(fabsf(e->operand(4)->approximate(globalContext) -(1.0f/6.0f)) <= 0.00001f);
  assert(fabsf(e->operand(5)->approximate(globalContext) -(-1.0f/4.0f)) <= 0.00001f);
  assert(fabsf(e->operand(6)->approximate(globalContext) -(1.0f/24.0f)) <= 0.00001f);
  assert(fabsf(e->operand(7)->approximate(globalContext) -(-1.0f/12.0f)) <= 0.00001f);
  assert(fabsf(e->operand(8)->approximate(globalContext) -(1.0f/24.0f)) <= 0.00001f);
  delete a;
  delete e;

  char productText[20] ={'p', 'r', 'o', 'd', 'u', 'c', 't', '(', '2', '+', 'n', '*', Ion::Charset::IComplex, ',', '1', ',','5',')', 0};
  a = Expression::parse(productText);
  e = a->evaluate(globalContext);
  assert(fabsf(-100.0f - ((Complex *)e)->a()) <= 0.00001f &&
         fabsf(-540.0f - ((Complex *)e)->b()) <= 0.00001f);
  delete a;
  delete e;

  char rooText1[20] ={'r','o','o','t', '(', '3', '+', Ion::Charset::IComplex, ',', '3',')', 0};
  a = Expression::parse(rooText1);
  e = a->evaluate(globalContext);
  assert(fabsf(1.4593656008f - ((Complex *)e)->a()) <= 0.00001f &&
         fabsf(0.1571201229f - ((Complex *)e)->b()) <= 0.00001f);
  delete e;

  char rooText2[20] ={'r','o','o','t', '(', '3', ',','3', '+', Ion::Charset::IComplex, ')', 0};
  a = Expression::parse(rooText2);
  e = a->evaluate(globalContext);
  assert(fabsf(1.38200696233f - ((Complex *)e)->a()) <= 0.00001f &&
         fabsf(-0.152442779f - ((Complex *)e)->b()) <= 0.00001f);
  delete e;


  char text2[20] ={Ion::Charset::Root, '(', '3', '+', Ion::Charset::IComplex, ',', '3',')', 0};
  a = Expression::parse(text2);
  e = a->evaluate(globalContext);
  assert((1.75532f - 0.00001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (1.75532f + 0.00001f) &&
      0.28485f - 0.00001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <= 0.28485f + 0.00001f);
  delete a;
  delete e;

  char sumText[20] ={'s', 'u', 'm', '(', '2', '+', 'n', '*', Ion::Charset::IComplex, ',', '1', ',','5',')', 0};
  a = Expression::parse(sumText);
  e = a->evaluate(globalContext);
  assert(fabsf(10.0f - ((Complex *)e)->a()) <= 0.00001f &&
         fabsf(15.0f - ((Complex *)e)->b()) <= 0.00001f);
  delete a;
  delete e;

  a = Expression::parse("transpose([[1,2,3][4,5,-6][7,8,9]])");
  e = a->evaluate(globalContext);
  assert(fabsf(e->operand(0)->approximate(globalContext)-1.0f) <= 0.00001f);
  assert(fabsf(e->operand(1)->approximate(globalContext) -4.0f) <= 0.00001f);
  assert(fabsf(e->operand(2)->approximate(globalContext) -7.0f) <= 0.00001f);
  assert(fabsf(e->operand(3)->approximate(globalContext) -2.0f) <= 0.00001f);
  assert(fabsf(e->operand(4)->approximate(globalContext) -5.0f) <= 0.00001f);
  assert(fabsf(e->operand(5)->approximate(globalContext) -8.0f) <= 0.00001f);
  assert(fabsf(e->operand(6)->approximate(globalContext) -3.0f) <= 0.00001f);
  assert(fabsf(e->operand(7)->approximate(globalContext) -(-6.0f)) <= 0.00001f);
  assert(fabsf(e->operand(8)->approximate(globalContext) -9.0f) <= 0.00001f);
  delete a;
  delete e;
}
