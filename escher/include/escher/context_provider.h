#ifndef ESCHER_CONTEXT_PROVIDER_H
#define ESCHER_CONTEXT_PROVIDER_H

#include <poincare/context.h>

class ContextProvider {
public:
  virtual Poincare::Context * context() const { return nullptr; }
};

#endif
