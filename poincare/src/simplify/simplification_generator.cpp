#include "simplification_generator.h"
#include <poincare/integer.h>
extern "C" {
#include <assert.h>
}

Expression * SimplificationGenerator::AddIntegers(Expression ** parameters, int numberOfParameters) {
  Integer * result = new Integer(0);
  for (int i=0; i<numberOfParameters; i++) {
    assert(parameters[i]->type() == Expression::Type::Integer);
    // FIXME: get rid of this operator overloading.
    *result = *result + *(Integer *)parameters[i];
    delete parameters[i];
  }
  return result;
}
