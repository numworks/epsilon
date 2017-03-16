#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_fraction_approximate) {
  GlobalContext globalContext;
  Expression * f = Expression::parse("1/2");
  assert(f->approximate(globalContext) == 0.5f);
  delete f;
}

QUIZ_CASE(poincare_fraction_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1/2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 0.5f);
  delete a;
  delete e;

  char expText1[50] ={'(','3','+',Ion::Charset::IComplex,')', '/', '(','4', '+', Ion::Charset::IComplex, ')',0};
  a = Expression::parse(expText1);
  e = a->evaluate(globalContext);
  assert(((Complex *)e)->a() == 13.0f/17.0f && ((Complex *)e)->b() == 1.0f/17.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]/2");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 0.5f);
  assert(e->operand(1)->approximate(globalContext) == 1.0f);
  assert(e->operand(2)->approximate(globalContext) == 1.5f);
  assert(e->operand(3)->approximate(globalContext) == 2.0f);
  assert(e->operand(4)->approximate(globalContext) == 2.5f);
  assert(e->operand(5)->approximate(globalContext) == 3.0f);
  delete a;
  delete e;

  char expText2[100] ={'[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']','/','(','4','+',Ion::Charset::IComplex, ')',0};
  a = Expression::parse(expText2);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 4.0f/17.0f && ((Complex *)e->operand(0))->b() == -1.0f/17.0f);
  assert(((Complex *)e->operand(1))->a() == 9.0f/17.0f && ((Complex *)e->operand(1))->b() == 2.0f/17.0f);
  assert(((Complex *)e->operand(2))->a() == 12.0f/17.0f && ((Complex *)e->operand(2))->b() == -3.0f/17.0f);
  assert(((Complex *)e->operand(3))->a() == 16.0f/17.0f && ((Complex *)e->operand(3))->b() == -4.0f/17.0f);
  assert(((Complex *)e->operand(4))->a() == 20.0f/17.0f && ((Complex *)e->operand(4))->b() == -5.0f/17.0f);
  assert(((Complex *)e->operand(5))->a() == 24.0f/17.0f && ((Complex *)e->operand(5))->b() == -6.0f/17.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4]]/[[3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(fabsf(e->operand(0)->approximate(globalContext) - 2.0f) < 0.000001f);
  assert(fabsf(e->operand(1)->approximate(globalContext) - -1.0f) < 0.000001f);
  assert(fabsf(e->operand(2)->approximate(globalContext) - 1.0f) < 0.000001f);
  assert(fabsf(e->operand(3)->approximate(globalContext) - 0.0f) < 0.000001f);
  delete a;
  delete e;
}
