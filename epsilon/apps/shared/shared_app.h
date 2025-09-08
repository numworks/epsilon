#pragma once

#include <escher/app.h>
#include <escher/nested_menu_controller.h>
#include <poincare/symbol_context.h>

namespace Shared {

class SharedApp : public Escher::App {
 public:
  class Snapshot : public Escher::App::Snapshot {
   public:
    void tidy() override;
    void reset() override;
  };
  const Poincare::SymbolContext& localContext() const override;

 protected:
  SharedApp(Snapshot* snapshot, Escher::ViewController* rootViewController);
};

}  // namespace Shared
