#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_subtraction_approximate) {
  GlobalContext globalContext;
  Expression * s = Expression::parse("1-2");
  assert(s->approximate(globalContext) == -1.0f);
  delete s;
}

QUIZ_CASE(poincare_substraction_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1-2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == -1.0f);
  delete a;
  delete e;

  char expText1[10] ={'3','+',Ion::Charset::SmallIota, '-', '(', '4', '+', Ion::Charset::SmallIota, ')', 0};
  a = Expression::parse(expText1);
  e = a->evaluate(globalContext);
  assert(((Complex *)e)->a() == -1.0f && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]-3");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == -2.0f);
  assert(e->operand(1)->approximate(globalContext) == -1.0f);
  assert(e->operand(2)->approximate(globalContext) == 0.0f);
  assert(e->operand(3)->approximate(globalContext) == 1.0f);
  assert(e->operand(4)->approximate(globalContext) == 2.0f);
  assert(e->operand(5)->approximate(globalContext) == 3.0f);
  delete a;
  delete e;

  char expText2[100] ={'[','[','1',',','2','+', Ion::Charset::SmallIota,']','[','3',',','4',']','[','5',',','6',']',']','-','(', '4', '+', Ion::Charset::SmallIota, ')', 0};
  a = Expression::parse(expText2);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == -3.0f && ((Complex *)e->operand(0))->b() == -1.0f);
  assert(((Complex *)e->operand(1))->a() == -2.0f && ((Complex *)e->operand(1))->b() == 0.0f);
  assert(((Complex *)e->operand(2))->a() == -1.0f && ((Complex *)e->operand(2))->b() == -1.0f);
  assert(((Complex *)e->operand(3))->a() == 0.0f && ((Complex *)e->operand(3))->b() == -1.0f);
  assert(((Complex *)e->operand(4))->a() == 1.0f && ((Complex *)e->operand(4))->b() == -1.0f);
  assert(((Complex *)e->operand(5))->a() == 2.0f && ((Complex *)e->operand(5))->b() == -1.0f);
  delete a;
  delete e;

  a = Expression::parse("3-[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 2.0f);
  assert(e->operand(1)->approximate(globalContext) == 1.0f);
  assert(e->operand(2)->approximate(globalContext) == 0.0f);
  assert(e->operand(3)->approximate(globalContext) == -1.0f);
  assert(e->operand(4)->approximate(globalContext) == -2.0f);
  assert(e->operand(5)->approximate(globalContext) == -3.0f);
  delete a;
  delete e;

  char expText3[100] ={'3','+',Ion::Charset::SmallIota, '-','[','[','1',',','2','+', Ion::Charset::SmallIota,']','[','3',',','4',']','[','5',',','6',']',']', 0};
  a = Expression::parse(expText3);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 2.0f && ((Complex *)e->operand(0))->b() == 1.0f);
  assert(((Complex *)e->operand(1))->a() == 1.0f && ((Complex *)e->operand(1))->b() == 0.0f);
  assert(((Complex *)e->operand(2))->a() == 0.0f && ((Complex *)e->operand(2))->b() == 1.0f);
  assert(((Complex *)e->operand(3))->a() == -1.0f && ((Complex *)e->operand(3))->b() == 1.0f);
  assert(((Complex *)e->operand(4))->a() == -2.0f && ((Complex *)e->operand(4))->b() == 1.0f);
  assert(((Complex *)e->operand(5))->a() == -3.0f && ((Complex *)e->operand(5))->b() == 1.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == -5.0f);
  assert(e->operand(1)->approximate(globalContext) == -3.0f);
  assert(e->operand(2)->approximate(globalContext) == -1.0f);
  assert(e->operand(3)->approximate(globalContext) == 1.0f);
  assert(e->operand(4)->approximate(globalContext) == 3.0f);
  assert(e->operand(5)->approximate(globalContext) == 5.0f);
  delete a;
  delete e;

  char expText4[100] ={'[','[','1',',','2','+', Ion::Charset::SmallIota,']','[','3',',','4',']','[','5',',','6',']',']','-',
    '[','[','1',',','2','+', Ion::Charset::SmallIota,']','[','3',',','4',']','[','5',',','6','+', Ion::Charset::SmallIota,']',']',0};
  a = Expression::parse(expText4);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 0.0f && ((Complex *)e->operand(0))->b() == 0.0f);
  assert(((Complex *)e->operand(1))->a() == 0.0f && ((Complex *)e->operand(1))->b() == 0.0f);
  assert(((Complex *)e->operand(2))->a() == 0.0f && ((Complex *)e->operand(2))->b() == 0.0f);
  assert(((Complex *)e->operand(3))->a() == 0.0f && ((Complex *)e->operand(3))->b() == 0.0f);
  assert(((Complex *)e->operand(4))->a() == 0.0f && ((Complex *)e->operand(4))->b() == 0.0f);
  assert(((Complex *)e->operand(5))->a() == 0.0f && ((Complex *)e->operand(5))->b() == -1.0f);
  delete a;
  delete e;
}
