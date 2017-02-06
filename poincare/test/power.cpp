#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_power_approximate) {
  GlobalContext globalContext;
  Expression * p = Expression::parse("2^3");
  assert(p->approximate(globalContext) == 8.0f);
  delete p;
}

QUIZ_CASE(poincare_power_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("2^3");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 8.0f);
  delete a;
  delete e;

  char expText1[15] ={'(','3','+',Ion::Charset::SmallIota, ')', '^', '4', 0};
  a = Expression::parse(expText1);
  e = a->evaluate(globalContext);
  assert(28.0f - 0.00001f <= (((Complex *)e)->a()) && (((Complex *)e)->a()) <= 28.0f + 0.00001f &&
          96.0f - 0.00001f <= (((Complex *)e)->b()) && (((Complex *)e)->b()) <= 96.0f + 0.00001f);
  delete a;
  delete e;

  char expText2[15] ={'4', '^','(','3','+',Ion::Charset::SmallIota, ')', 0};
  a = Expression::parse(expText2);
  e = a->evaluate(globalContext);
  assert(((Complex *)e)->a() == 11.7412464f && ((Complex *)e)->b() == 62.9137754f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4]]^3");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 37.0f);
  assert(e->operand(1)->approximate(globalContext) == 54.0f);
  assert(e->operand(2)->approximate(globalContext) == 81.0f);
  assert(e->operand(3)->approximate(globalContext) == 118.0f);
  delete a;
  delete e;
}
