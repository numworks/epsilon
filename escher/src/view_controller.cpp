#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <string.h>

namespace Escher {

void ViewController::stackOpenPage(ViewController* nextPage) {
  StackViewController* stackViewControllerResponder =
      static_cast<StackViewController*>(parentResponder());
  assert(stackViewControllerResponder != nullptr);
  stackViewControllerResponder->push(nextPage);
}

bool ViewController::popFromStackViewControllerOnLeftEvent(
    Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    StackViewController* stackViewControllerResponder =
        static_cast<StackViewController*>(parentResponder());
    assert(stackViewControllerResponder != nullptr);
    stackViewControllerResponder->pop();
    return true;
  }
  return false;
}

}  // namespace Escher
