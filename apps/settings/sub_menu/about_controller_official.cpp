#include "about_controller.h"

namespace Settings {

void AboutController::viewWillAppear() {
  GenericSubController::viewWillAppear();
  m_view.setMessages(nullptr, 0);
}

}
