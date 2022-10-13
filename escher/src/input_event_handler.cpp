#include <escher/input_event_handler.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/container.h>
#include <escher/toolbox.h>
#include <escher/metric.h>

namespace Escher {

bool InputEventHandler::handleBoxEvent(Ion::Events::Event event) {
  if (m_inputEventHandlerDelegate == nullptr) {
    return false;
  }
  NestedMenuController * box = nullptr;
  if (event == Ion::Events::Toolbox) {
    box = m_inputEventHandlerDelegate->toolboxForInputEventHandler();
  } else if (event == Ion::Events::Var) {
    box = m_inputEventHandlerDelegate->variableBoxForInputEventHandler();
  }
  if (box) {
    box->setSender(this);
    Container::activeApp()->displayModalViewController(box, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  return false;
}

}
