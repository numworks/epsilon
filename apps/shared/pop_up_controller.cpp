#include "pop_up_controller.h"
#include <escher/container.h>
#include <escher/metric.h>

namespace Shared {

PopUpController::PopUpController(Escher::Invocation OkInvocation, std::initializer_list<I18n::Message> messages) : Escher::MessagePopUpController(messages.size(), OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel) {
  int index = 0;
  for (I18n::Message message : messages) {
    setContentMessage(index++, message);
  }
}

void PopUpController::presentModally() {
  Escher::Container::activeApp()->displayModalViewController(this, 0.f, 0.f, Escher::Metric::PopUpTopMargin, Escher::Metric::PopUpRightMargin, Escher::Metric::PopUpBottomMargin, Escher::Metric::PopUpLeftMargin);
}

}
