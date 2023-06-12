#ifndef SHARED_APP_H
#define SHARED_APP_H

#include <escher/app.h>
#include <escher/nested_menu_controller.h>

namespace Shared {

class SharedApp : public Escher::App {
 public:
  class Snapshot : public Escher::App::Snapshot {
   public:
    void tidy() override;
    void reset() override;
  };
  Poincare::Context* localContext() override;

 protected:
  SharedApp(Snapshot* snapshot, Escher::ViewController* rootViewController);
};

}  // namespace Shared
#endif
