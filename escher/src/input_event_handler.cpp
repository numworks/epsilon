#include <escher/boxes_delegate.h>
#include <escher/container.h>
#include <escher/input_event_handler.h>
#include <escher/metric.h>
#include <escher/toolbox.h>

namespace Escher {

bool InputEventHandler::handleBoxEvent(Ion::Events::Event event) {
  if (m_boxesDelegate == nullptr) {
    return false;
  }
  PervasiveBox* box = nullptr;
  if (event == Ion::Events::Toolbox) {
    box = m_boxesDelegate->toolbox();
  } else if (event == Ion::Events::Var) {
    box = m_boxesDelegate->variableBox();
  }
  if (!box) {
    return false;
  }
  box->setSender(this);
  box->open();
  return true;
}

}  // namespace Escher
