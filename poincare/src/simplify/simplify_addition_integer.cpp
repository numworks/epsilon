#include "simplify_addition_integer.h"
#include <poincare/addition.h>
#include <poincare/integer.h>
extern "C" {
#include <stdlib.h>
}

Expression * SimplifyAdditionInteger(Expression * e) {
  if (e->type() != Expression::Type::Addition) {
    return nullptr;
  }
  Addition * a = (Addition *)e;
  // Let's figure out which children of a are integers and which are not
  Integer ** integers = (Integer **)malloc(sizeof(Integer *) * a->numberOfOperands());
  int numberOfIntegers = 0;
  Expression ** others = (Expression **)malloc(sizeof(Expression *) * a->numberOfOperands());
  int numberOfOthers = 0;
  for (int i=0; i<a->numberOfOperands(); i++) {
    Expression * term = a->operand(i);
    if (term->type() == Expression::Type::Integer) {
      integers[numberOfIntegers++] = (Integer *)term;
    } else {
      others[numberOfOthers++] = term;
    }
  }
  Expression * result = nullptr;
  if (numberOfIntegers >= 2) {
    Integer sum = Integer((native_int_t)0);
    for (int i=0; i<numberOfIntegers; i++) {
      sum = sum + *integers[i]; // FIXME: Let's remove operator overloading, ok?
    }
    if (numberOfOthers == 0) {
      result = sum.clone();
    } else {
      others[numberOfOthers++] = &sum;
      result = new Addition(others, numberOfOthers, true);
    }
  }
  free(others);
  free(integers);
  return result;
}
