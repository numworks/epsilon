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

  e = Expression::parse("diff(2*x, 2)");
  assert(e->type() == Expression::Type::Derivative);
  delete e;

  e = Expression::parse("int(x, 2, 3)");
  assert(e->type() == Expression::Type::Integral);
  delete e;

  e = Expression::parse("log(2)");
  assert(e->type() == Expression::Type::Logarithm);
  delete e;

  e = Expression::parse("ln(2)");
  assert(e->type() == Expression::Type::NaperianLogarithm);
  delete e;

  e = Expression::parse("root(2,3)");
  assert(e->type() == Expression::Type::NthRoot);
  delete e;

  char text[5] = {Ion::Charset::Root, '(', '2', ')', 0};
  e = Expression::parse(text);
  assert(e->type() == Expression::Type::SquareRoot);
  delete e;

  e = Expression::parse("det([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::Determinant);
  delete e;

  e = Expression::parse("inverse([[1,2,3][4,5,6][7,8,9]])");
  assert(e->type() == Expression::Type::MatrixInverse);
  delete e;
}

QUIZ_CASE(poincare_function_approximate) {
  GlobalContext globalContext;
  Expression * e = Expression::parse("abs(-1)");
  assert(e->approximate(globalContext) == 1.0f);
  delete e;

  e = Expression::parse("diff(2*x, 2)");
  assert(e->approximate(globalContext) == 2.0f);
  delete e;

  e = Expression::parse("log(2)");
  assert(e->approximate(globalContext) == log10f(2.0f));
  delete e;

  e = Expression::parse("ln(2)");
  assert(e->approximate(globalContext) == logf(2.0f));
  delete e;

  e = Expression::parse("root(2,3)");
  assert(e->approximate(globalContext) == powf(2.0f, 1.0f/3.0f));
  delete e;

  char text[5] = {Ion::Charset::Root, '(', '2', ')', 0};
  e = Expression::parse(text);
  assert(e->approximate(globalContext) == sqrtf(2.0f));
  delete e;

  e = Expression::parse("det([[1,2,3][4,5,-6][7,8,9]])");
  assert(fabsf(e->approximate(globalContext)-(-72.0f)) <= 0.00001f);
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

  char text1[20] ={'r','o', 'o','t', '(', '3', '+', Ion::Charset::IComplex, ',', '3',')', 0};
  a = Expression::parse(text1);
  e = a->evaluate(globalContext);
  assert((1.45937f - 0.00001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (1.45937f + 0.00001f) &&
      0.15712f - 0.00001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <= 0.15712f + 0.00001f);
  delete a;
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
}
