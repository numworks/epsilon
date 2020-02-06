#include "about_controller.h"
#include "selectable_view_with_messages.h"

namespace Settings {

View * AboutController::view() {
  static I18n::Message cautionMessages[] = {I18n::Message::AboutWarning1, I18n::Message::AboutWarning2, I18n::Message::AboutWarning3, I18n::Message::AboutWarning4};
  static SelectableViewWithMessages view(&m_selectableTableView, cautionMessages, 4);
  return &view;
}

}
