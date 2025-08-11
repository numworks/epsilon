#ifndef POINCARE_VARIABLE_STORE_H
#define POINCARE_VARIABLE_STORE_H

#include <assert.h>

#include "context.h"

/*  This file is temporary! Store is only performed in the Apps. Poincare only
 * has a const Context available */

namespace Poincare {

/* A VariableStore allows to access variable types and definitions and to insert
 * new entries */
class VariableStore : public Context {
 public:
  // TODO: the parameters should have Poincare::Expression types
  virtual bool setExpressionForUserNamed(const Internal::Tree* expression,
                                         const Internal::Tree* symbol) = 0;
};

}  // namespace Poincare

#endif
