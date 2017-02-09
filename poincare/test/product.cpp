#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_product_approximate) {
  GlobalContext globalContext;
  Expression * p = Expression::parse("1*2");
  assert(p->approximate(globalContext) == 2.0f);
  delete p;
}

QUIZ_CASE(poincare_product_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1*2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 2.0f);
  delete a;
  delete e;

  char expText1[15] ={'(','3','+',Ion::Charset::IComplex, ')', '*', '(','4', '+', Ion::Charset::IComplex, ')', 0};
  a = Expression::parse(expText1);
  e = a->evaluate(globalContext);
  assert(((Complex *)e)->a() == 11.0f && ((Complex *)e)->b() == 7.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]*2");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 2.0f);
  assert(e->operand(1)->approximate(globalContext) == 4.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 8.0f);
  assert(e->operand(4)->approximate(globalContext) == 10.0f);
  assert(e->operand(5)->approximate(globalContext) == 12.0f);
  delete a;
  delete e;


  char expText2[100] ={'[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']','*', '(','3','+',Ion::Charset::IComplex, ')', 0};
  a = Expression::parse(expText2);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 3.0f && ((Complex *)e->operand(0))->b() == 1.0f);
  assert(((Complex *)e->operand(1))->a() == 5.0f && ((Complex *)e->operand(1))->b() == 5.0f);
  assert(((Complex *)e->operand(2))->a() == 9.0f && ((Complex *)e->operand(2))->b() == 3.0f);
  assert(((Complex *)e->operand(3))->a() == 12.0f && ((Complex *)e->operand(3))->b() == 4.0f);
  assert(((Complex *)e->operand(4))->a() == 15.0f && ((Complex *)e->operand(4))->b() == 5.0f);
  assert(((Complex *)e->operand(5))->a() == 18.0f && ((Complex *)e->operand(5))->b() == 6.0f);
  delete a;
  delete e;

  a = Expression::parse("3*[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 3.0f);
  assert(e->operand(1)->approximate(globalContext) == 6.0f);
  assert(e->operand(2)->approximate(globalContext) == 9.0f);
  assert(e->operand(3)->approximate(globalContext) == 12.0f);
  assert(e->operand(4)->approximate(globalContext) == 15.0f);
  assert(e->operand(5)->approximate(globalContext) == 18.0f);
  delete a;
  delete e;

  char expText3[100] ={'(','3','+',Ion::Charset::IComplex, ')','*','[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']', 0};
  a = Expression::parse(expText3);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 3.0f && ((Complex *)e->operand(0))->b() == 1.0f);
  assert(((Complex *)e->operand(1))->a() == 5.0f && ((Complex *)e->operand(1))->b() == 5.0f);
  assert(((Complex *)e->operand(2))->a() == 9.0f && ((Complex *)e->operand(2))->b() == 3.0f);
  assert(((Complex *)e->operand(3))->a() == 12.0f && ((Complex *)e->operand(3))->b() == 4.0f);
  assert(((Complex *)e->operand(4))->a() == 15.0f && ((Complex *)e->operand(4))->b() == 5.0f);
  assert(((Complex *)e->operand(5))->a() == 18.0f && ((Complex *)e->operand(5))->b() == 6.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 11.0f);
  assert(e->operand(1)->approximate(globalContext) == 14.0f);
  assert(e->operand(2)->approximate(globalContext) == 17.0f);
  assert(e->operand(3)->approximate(globalContext) == 20.0f);
  assert(e->operand(4)->approximate(globalContext) == 23.0f);
  assert(e->operand(5)->approximate(globalContext) == 30.0f);
  assert(e->operand(6)->approximate(globalContext) == 37.0f);
  assert(e->operand(7)->approximate(globalContext) == 44.0f);
  assert(e->operand(8)->approximate(globalContext) == 35.0f);
  assert(e->operand(9)->approximate(globalContext) == 46.0f);
  assert(e->operand(10)->approximate(globalContext) == 57.0f);
  assert(e->operand(11)->approximate(globalContext) == 68.0f);
  delete a;
  delete e;

  char expText4[100] ={'[','[','1',',','2','+', Ion::Charset::IComplex,']','[','3',',','4',']','[','5',',','6',']',']','*',
    '[','[','1',',','2','+', Ion::Charset::IComplex,',','3',',','4',']','[','5',',','6','+', Ion::Charset::IComplex,',', '7',',','8',']',']',0};
  a = Expression::parse(expText4);
  e = a->evaluate(globalContext);
  assert(((Complex *)e->operand(0))->a() == 11.0f && ((Complex *)e->operand(0))->b() == 5.0f);
  assert(((Complex *)e->operand(1))->a() == 13.0f && ((Complex *)e->operand(1))->b() == 9.0f);
  assert(((Complex *)e->operand(2))->a() == 17.0f && ((Complex *)e->operand(2))->b() == 7.0f);
  assert(((Complex *)e->operand(3))->a() == 20.0f && ((Complex *)e->operand(3))->b() == 8.0f);
  assert(((Complex *)e->operand(4))->a() == 23.0f && ((Complex *)e->operand(4))->b() == 0.0f);
  assert(((Complex *)e->operand(5))->a() == 30.0f && ((Complex *)e->operand(5))->b() == 7.0f);
  assert(((Complex *)e->operand(6))->a() == 37.0f && ((Complex *)e->operand(6))->b() == 0.0f);
  assert(((Complex *)e->operand(7))->a() == 44.0f && ((Complex *)e->operand(7))->b() == 0.0f);
  assert(((Complex *)e->operand(8))->a() == 35.0f && ((Complex *)e->operand(8))->b() == 0.0f);
  assert(((Complex *)e->operand(9))->a() == 46.0f && ((Complex *)e->operand(9))->b() == 11.0f);
  assert(((Complex *)e->operand(10))->a() == 57.0f && ((Complex *)e->operand(10))->b() == 0.0f);
  assert(((Complex *)e->operand(11))->a() == 68.0f && ((Complex *)e->operand(11))->b() == 0.0f);
  delete a;
  delete e;
}
