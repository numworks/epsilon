#include "about_controller.h"
#include "selectable_view_with_messages.h"

namespace Settings {

void AboutController::viewWillAppear() {
  GenericSubController::viewWillAppear();
  I18n::Message cautionMessages[] = {I18n::Message::AboutWarning1, I18n::Message::AboutWarning2, I18n::Message::AboutWarning3, I18n::Message::AboutWarning4};
  m_view.setMessages(cautionMessages, sizeof(cautionMessages)/sizeof(I18n::Message));
}

}
