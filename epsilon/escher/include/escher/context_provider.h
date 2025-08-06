#ifndef ESCHER_CONTEXT_PROVIDER_H
#define ESCHER_CONTEXT_PROVIDER_H

#include <escher/container.h>
#include <poincare/variable_store.h>

namespace Escher {

class ContextProvider {
 public:
  // TODO: rename method
  Poincare::VariableStore* context() const {
    assert(Escher::App::app());
    return Escher::App::app()->localContext();
  }
};
}  // namespace Escher

#endif
