#include <quiz.h>
#include <poincare.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_integer) {
  assert(Integer(123) == Integer(123));
  assert(Integer((int64_t)1234567891011121314) == Integer((int64_t)1234567891011121314));
  assert(Integer("123") == Integer(123));
  assert(!(Integer("-123") == Integer(123)));
  assert(Integer("-123") == Integer(-123));
  //assert(Integer("0123") == Integer(123));
  //FIXME: assert(Integer("0x2BABE") == Integer(178878));
  //FIXME: assert(Integer("0b1011") == Integer(11));
}

QUIZ_CASE(poincare_integer_compare) {
  assert(Integer(123) < Integer(456));
  assert(!(Integer(123) < Integer(123)));
  assert(!(Integer(-123) < Integer(-123)));
  assert(Integer(-100) < Integer(2));
  assert(Integer(-200) < Integer(-100));
  assert(Integer(123) < Integer("123456789123456789"));
  assert(Integer("123456789123456788") < Integer("123456789123456789"));
  assert(!(Integer("123456789123456789") < Integer("123456789123456788")));
}

QUIZ_CASE(poincare_integer_add) {
  assert(Integer("0").add(Integer("0")) == Integer("0"));
  assert(Integer(123).add(Integer(456)) == Integer(579));
  assert(Integer("123456789123456789").add(Integer(1)) == Integer("123456789123456790"));
}

QUIZ_CASE(poincare_integer_subtract) {
  assert(Integer(123).subtract(Integer(23)) == Integer(100));
  assert(Integer("123456789123456789").subtract(Integer("9999999999")) == Integer("123456779123456790"));
  assert(Integer(23).subtract(Integer(100)) == Integer(-77));
}

QUIZ_CASE(poincare_integer_multiply) {
  assert(Integer(12).multiply_by(Integer(34)) == Integer(408));
  assert(Integer(56).multiply_by(Integer("0")) == Integer("0"));
  assert(Integer(-12).multiply_by(Integer(34)) == Integer(-408));
  assert(Integer(-12).multiply_by(Integer(-34)) == Integer(408));
  assert(Integer(999999).multiply_by(Integer(999999)) == Integer("999998000001"));
  assert(Integer("9999999999").multiply_by(Integer("9999999999")) == Integer("99999999980000000001"));
}

QUIZ_CASE(poincare_integer_divide) {
  assert(Integer(8).divide_by(Integer(4)) == Integer(2));
  assert(Integer("3293920983029832").divide_by(Integer("38928")) == Integer("84615726033"));
  assert(Integer("3293920983029832").divide_by(Integer("389090928")) == Integer("8465684"));
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
