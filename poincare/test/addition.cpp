#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_addition_approximate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1+2");
  assert(a->approximate(globalContext) == 3.0f);
  delete a;
}

QUIZ_CASE(poincare_addition_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1+2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 3.0f);
  delete a;
  delete e;

  char expText1[8] ={'3','+',Ion::Charset::IComplex, '+', '4', '+', Ion::Charset::IComplex, 0};
  a = Expression::parse(expText1);
  e = a->evaluate(globalContext);
  assert(((Complex *)e)->a() == 7.0f && ((Complex *)e)->b() == 2.0f);
  delete a;
  delete e;

#if MATRICES_ARE_DEFINED
  a = Expression::parse("[[1,2][3,4][5,6]]+3");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 4.0f);
  assert(e->operand(1)->approximate(globalContext) == 5.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 7.0f);
  assert(e->operand(4)->approximate(globalContext) == 8.0f);
  assert(e->operand(5)->approximate(globalContext) == 9.0f);
  delete a;
  delete e;

  char expText2[100] ={'[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']','+','3','+',Ion::Charset::IComplex, 0};
  a = Expression::parse(expText2);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 4.0f && ((Complex *)e->operand(0))->b() == 1.0f);
  assert(((Complex *)e->operand(1))->a() == 5.0f && ((Complex *)e->operand(1))->b() == 2.0f);
  assert(((Complex *)e->operand(2))->a() == 6.0f && ((Complex *)e->operand(2))->b() == 1.0f);
  assert(((Complex *)e->operand(3))->a() == 7.0f && ((Complex *)e->operand(3))->b() == 1.0f);
  assert(((Complex *)e->operand(4))->a() == 8.0f && ((Complex *)e->operand(4))->b() == 1.0f);
  assert(((Complex *)e->operand(5))->a() == 9.0f && ((Complex *)e->operand(5))->b() == 1.0f);
  delete a;
  delete e;

  a = Expression::parse("3+[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 4.0f);
  assert(e->operand(1)->approximate(globalContext) == 5.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 7.0f);
  assert(e->operand(4)->approximate(globalContext) == 8.0f);
  assert(e->operand(5)->approximate(globalContext) == 9.0f);
  delete a;
  delete e;

  char expText3[100] ={'3','+',Ion::Charset::IComplex, '+','[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']', 0};
  a = Expression::parse(expText3);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 4.0f && ((Complex *)e->operand(0))->b() == 1.0f);
  assert(((Complex *)e->operand(1))->a() == 5.0f && ((Complex *)e->operand(1))->b() == 2.0f);
  assert(((Complex *)e->operand(2))->a() == 6.0f && ((Complex *)e->operand(2))->b() == 1.0f);
  assert(((Complex *)e->operand(3))->a() == 7.0f && ((Complex *)e->operand(3))->b() == 1.0f);
  assert(((Complex *)e->operand(4))->a() == 8.0f && ((Complex *)e->operand(4))->b() == 1.0f);
  assert(((Complex *)e->operand(5))->a() == 9.0f && ((Complex *)e->operand(5))->b() == 1.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 2.0f);
  assert(e->operand(1)->approximate(globalContext) == 4.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 8.0f);
  assert(e->operand(4)->approximate(globalContext) == 10.0f);
  assert(e->operand(5)->approximate(globalContext) == 12.0f);
  delete a;
  delete e;

  char expText4[100] ={'[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']','+',
    '[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6','+', Ion::Charset::IComplex,']',']',0};
  a = Expression::parse(expText4);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 2.0f && ((Complex *)e->operand(0))->b() == 0.0f);
  assert(((Complex *)e->operand(1))->a() == 4.0f && ((Complex *)e->operand(1))->b() == 2.0f);
  assert(((Complex *)e->operand(2))->a() == 6.0f && ((Complex *)e->operand(2))->b() == 0.0f);
  assert(((Complex *)e->operand(3))->a() == 8.0f && ((Complex *)e->operand(3))->b() == 0.0f);
  assert(((Complex *)e->operand(4))->a() == 10.0f && ((Complex *)e->operand(4))->b() == 0.0f);
  assert(((Complex *)e->operand(5))->a() == 12.0f && ((Complex *)e->operand(5))->b() == 1.0f);
  delete a;
  delete e;
#endif
}
