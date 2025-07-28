#ifndef ESCHER_CONTEXT_PROVIDER_H
#define ESCHER_CONTEXT_PROVIDER_H

#include <escher/container.h>
#include <poincare/old/context.h>

namespace Escher {

class ContextProvider {
 public:
  Poincare::Context* context() const {
    assert(Escher::App::app());
    return Escher::App::app()->localContext();
  }
};
}  // namespace Escher

#endif
