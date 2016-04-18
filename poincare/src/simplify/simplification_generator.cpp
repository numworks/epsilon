#include "simplification_generator.h"
#include <poincare/integer.h>
extern "C" {
#include <assert.h>
}

Expression * SimplificationGenerator::AddIntegers(Expression ** parameters, int numberOfParameters) {
  Integer * result = new Integer((native_int_t)0);
  for (int i=0; i<numberOfParameters; i++) {
    assert(parameters[i]->type() == Expression::Type::Integer);
    *result = result->add(*(Integer *)parameters[i], false);
    /* Note We have to delete the parameters as they have been cloned before and
     * we are the last ones to use them here. */
    delete parameters[i];
  }
  return result;
}

Expression * SimplificationGenerator::MultiplyIntegers(Expression ** parameters, int numberOfParameters) {
  Integer * result = new Integer((native_int_t)1);
  for (int i=0; i<numberOfParameters; i++) {
    assert(parameters[i]->type() == Expression::Type::Integer);
    // FIXME: get rid of this operator overloading, there are to many stars.
    *result = *result * *(Integer *)parameters[i];
    /* Note We have to delete the parameters as they have been cloned before and
     * we are the last ones to use them here. */
    delete parameters[i];
  }
  return result;
}
