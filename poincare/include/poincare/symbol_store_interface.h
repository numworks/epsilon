#pragma once

#include <assert.h>

#include "symbol_context.h"

/* The SymbolStoreInterface is defined in Poincaré although no store is ever
 * performed in Poincaré. However it is useful to implement such an interface
 * for the Poincaré unit tests (see PoincareTest::SymbolStore). The projects
 * that use Poincaré can also implement this interface for their symbol store.
 */

namespace Poincare {

/* A SymbolStoreInterface allows to access variable types and definitions and
 * to insert new entries */
class SymbolStoreInterface : public SymbolContext {
 public:
  // TODO: the parameters should have Poincare::Expression types
  virtual bool setExpressionForUserNamed(const Internal::Tree* expression,
                                         const Internal::Tree* symbol) = 0;
};

}  // namespace Poincare
