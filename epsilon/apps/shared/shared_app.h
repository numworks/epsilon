#ifndef SHARED_APP_H
#define SHARED_APP_H

#include <escher/app.h>
#include <escher/nested_menu_controller.h>
#include <poincare/variable_store.h>

namespace Shared {

class SharedApp : public Escher::App {
 public:
  class Snapshot : public Escher::App::Snapshot {
   public:
    void tidy() override;
    void reset() override;
  };
  Poincare::VariableStore* localContext() override;

 protected:
  SharedApp(Snapshot* snapshot, Escher::ViewController* rootViewController);
};

}  // namespace Shared
#endif
