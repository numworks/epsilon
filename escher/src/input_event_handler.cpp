#include <escher/container.h>
#include <escher/input_event_handler.h>
#include <escher/metric.h>
#include <escher/toolbox.h>

namespace Escher {

bool InputEventHandler::handleBoxEvent(Ion::Events::Event event) {
  App* app = App::app();
  PervasiveBox* box = nullptr;
  if (event == Ion::Events::Toolbox) {
    box = app->toolbox();
  } else if (event == Ion::Events::Var) {
    box = app->variableBox();
  }
  if (!box) {
    return false;
  }
  box->setSender(this);
  box->open();
  return true;
}

}  // namespace Escher
