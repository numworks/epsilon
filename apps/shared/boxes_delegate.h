#ifndef SHARED_BOXES_DELEGATE_H
#define SHARED_BOXES_DELEGATE_H

#include <escher/container.h>

#include "shared_app.h"

namespace Shared {

class BoxesDelegate : public Escher::BoxesDelegate {
 public:
  Escher::PervasiveBox *toolbox() override {
    return boxesDelegateApp()->toolbox();
  }
  Escher::PervasiveBox *variableBox() override {
    return boxesDelegateApp()->variableBox();
  }

 private:
  SharedApp *boxesDelegateApp() const {
    return static_cast<SharedApp *>(Escher::Container::activeApp());
  }
};

}  // namespace Shared

#endif
