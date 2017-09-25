#include <quiz.h>
#include <poincare.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_integer) {
  assert(Integer(123).isEqualTo(Integer(123)));
  assert(Integer("123").isEqualTo(Integer(123)));
  assert(!Integer("-123").isEqualTo(Integer(123)));
  assert(Integer("-123").isEqualTo(Integer(-123)));
  //assert(Integer("0123") == Integer(123));
  //FIXME: assert(Integer("0x2BABE") == Integer(178878));
  //FIXME: assert(Integer("0b1011") == Integer(11));
}

QUIZ_CASE(poincare_integer_compare) {
  assert(Integer(123).isLowerThan(Integer(456)));
  assert(!Integer(123).isLowerThan(Integer(123)));
  assert(!Integer(-123).isLowerThan(Integer(-123)));
  assert(Integer(-100).isLowerThan(Integer(2)));
  assert(Integer(-200).isLowerThan(Integer(-100)));
  assert(Integer(123).isLowerThan(Integer("123456789123456789")));
  assert(Integer(-123).isLowerThan(Integer("123456789123456789")));
  assert(Integer("123456789123456788").isLowerThan(Integer("123456789123456789")));
  assert(Integer("-1234567891234567892109209109").isLowerThan(Integer("123456789123456789")));
  assert(!Integer("123456789123456789").isLowerThan(Integer("123456789123456788")));
}

QUIZ_CASE(poincare_integer_addition) {
  assert(Integer::Addition(Integer("0"), Integer("0")).isEqualTo(Integer(0)));
  assert(Integer::Addition(Integer(123), Integer(456)).isEqualTo(Integer(579)));
  assert(Integer::Addition(Integer("123456789123456789"), Integer(1)).isEqualTo(Integer("123456789123456790")));
}

QUIZ_CASE(poincare_integer_subtraction) {
  assert(Integer::Subtraction(Integer(123), Integer(23)).isEqualTo(Integer(100)));
  assert(Integer::Subtraction(Integer("123456789123456789"), Integer("9999999999")).isEqualTo(Integer("123456779123456790")));
  assert(Integer::Subtraction(Integer(23), Integer(100)).isEqualTo(Integer(-77)));
}

QUIZ_CASE(poincare_integer_multiplication) {
  assert(Integer::Multiplication(Integer(12), Integer(34)).isEqualTo(Integer(408)));
  assert(Integer::Multiplication(Integer(56), Integer(0)).isEqualTo(Integer(0)));
  assert(Integer::Multiplication(Integer(-12), Integer(34)).isEqualTo(Integer(-408)));
  assert(Integer::Multiplication(Integer(-12), Integer(-34)).isEqualTo(Integer(408)));
  assert(Integer::Multiplication(Integer(123456), Integer(654321)).isEqualTo(Integer("80779853376")));
  assert(Integer::Multiplication(Integer("9999999999"), Integer("9999999999")).isEqualTo(Integer("99999999980000000001")));
}

QUIZ_CASE(poincare_integer_divide) {
//  assert(Integer(8).divide_by(Integer(4)) == Integer(2));
//  assert(Integer("3293920983029832").divide_by(Integer("38928")) == Integer("84615726033"));
//  assert(Integer("3293920983029832").divide_by(Integer("389090928")) == Integer("8465684"));
}

template<typename T>
void assert_integer_evals_to(int i, T result) {
  GlobalContext globalContext;
  Evaluation<T> * m = Integer(i).evaluate<T>(globalContext);
  assert(m->complexOperand(0)->a() == result);
  assert(m->complexOperand(0)->b() == 0.0f);
  assert(m->numberOfOperands() == 1);
  delete m;
}

QUIZ_CASE(poincare_integer_evaluate) {
  assert_integer_evals_to(1, 1.0f);
  assert_integer_evals_to(12345678, 12345678.0);
  assert_integer_evals_to(0, 0.0f);
  assert_integer_evals_to(-0, 0.0);
  assert_integer_evals_to(-1, -1.0f);
  assert_integer_evals_to(12345678, 12345678.0);
}
