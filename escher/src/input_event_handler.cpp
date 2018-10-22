#include <escher/input_event_handler.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/app.h>
#include <escher/toolbox.h>
#include <escher/metric.h>

bool InputEventHandler::handleToolboxEvent(App * app) {
  if (inputEventHandlerDelegate() != nullptr) {
    Toolbox * toolbox = inputEventHandlerDelegate()->toolboxForInputEventHandler(this);
    toolbox->setSender(this);
    app->displayModalViewController(toolbox, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  return false;
}
