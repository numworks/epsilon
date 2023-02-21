#include <assert.h>
#include <escher/alternate_view_controller.h>
#include <escher/container.h>

namespace Escher {

/* AlternateViewController */

AlternateViewController::AlternateViewController(
    Responder *parentResponder, AlternateViewDelegate *delegate,
    std::initializer_list<ViewController *> viewControllers)
    : ViewController(parentResponder), m_delegate(delegate) {
  assert(viewControllers.size() <= k_maxNumberOfViewController);
  size_t index = 0;
  for (ViewController *viewController : viewControllers) {
    m_viewControllers[index++] = viewController;
  }
}

void AlternateViewController::viewWillAppear() {
  activeViewController()->viewWillAppear();
}

}  // namespace Escher
