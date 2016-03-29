#include "simplification_generator.h"
#include <poincare/integer.h>
extern "C" {
#include <assert.h>
}

Expression * SimplificationGenerator::AddIntegers(Expression ** parameters, int numberOfParameters) {
  Integer result = Integer(0);
  for (int i=0; i<numberOfParameters; i++) {
    assert(parameters[i]->type() == Expression::Type::Integer);
    result = result + *(Integer *)parameters[i];
  }
  return result.clone();
}
