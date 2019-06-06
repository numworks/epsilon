#include <escher/input_event_handler.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/app.h>
#include <escher/toolbox.h>
#include <escher/metric.h>

bool InputEventHandler::handleBoxEvent(Ion::Events::Event event) {
  NestedMenuController * box = nullptr;
  if (m_inputEventHandlerDelegate) {
    box = event == Ion::Events::Toolbox ? m_inputEventHandlerDelegate->toolboxForInputEventHandler(this) : box;
    box = event == Ion::Events::Var ? m_inputEventHandlerDelegate->variableBoxForInputEventHandler(this) : box;
  }
  if (box) {
    box->setSender(this);
    app()->displayModalViewController(box, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  return false;
}
