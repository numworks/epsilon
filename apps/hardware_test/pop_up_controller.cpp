#include "pop_up_controller.h"
#include "../apps_container.h"
#include <apps/i18n.h>

namespace HardwareTest {

PopUpController::PopUpController() :
  Escher::PopUpController(
    4,
    Escher::Invocation(
      [](void * context, void * sender) {
        AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
        bool switched = appsContainer->switchTo(appsContainer->hardwareTestAppSnapshot());
        assert(switched);
        (void) switched; // Silence compilation warning about unused variable.
        return true;
      }, this),
    I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel
  )
{
  m_contentView.setMessage(0, I18n::Message::HardwareTestLaunch1);
  m_contentView.setMessage(1, I18n::Message::HardwareTestLaunch2);
  m_contentView.setMessage(2, I18n::Message::HardwareTestLaunch3);
  m_contentView.setMessage(3, I18n::Message::HardwareTestLaunch4);
}

}
