#pragma once

#include <escher/container.h>
#include <poincare/context.h>

namespace Escher {

class ContextProvider {
 public:
  const Poincare::Context* context() const {
    assert(Escher::App::app());
    return &Escher::App::app()->localContext();
  }
};
}  // namespace Escher
