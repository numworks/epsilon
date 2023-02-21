#include <escher/container.h>
#include <escher/input_event_handler.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/metric.h>
#include <escher/toolbox.h>

namespace Escher {

bool InputEventHandler::handleBoxEvent(Ion::Events::Event event) {
  if (m_inputEventHandlerDelegate == nullptr) {
    return false;
  }
  PervasiveBox* box = nullptr;
  if (event == Ion::Events::Toolbox) {
    box = m_inputEventHandlerDelegate->toolbox();
  } else if (event == Ion::Events::Var) {
    box = m_inputEventHandlerDelegate->variableBox();
  }
  if (!box) {
    return false;
  }
  box->setSender(this);
  box->open();
  return true;
}

}  // namespace Escher
