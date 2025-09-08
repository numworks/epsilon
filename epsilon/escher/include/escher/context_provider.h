#pragma once

#include <escher/container.h>
#include <poincare/symbol_context.h>

namespace Escher {

class ContextProvider {
 public:
  const Poincare::SymbolContext* context() const {
    assert(Escher::App::app());
    return &Escher::App::app()->localContext();
  }
};
}  // namespace Escher
