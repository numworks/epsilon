#ifndef ESCHER_CONTEXT_PROVIDER_H
#define ESCHER_CONTEXT_PROVIDER_H

#include <escher/container.h>
#include <poincare/context.h>

namespace Escher {

class ContextProvider {
 public:
  Poincare::Context* context() const {
    assert(Escher::Container::activeApp());
    return Escher::Container::activeApp()->localContext();
  }
};
}  // namespace Escher

#endif
