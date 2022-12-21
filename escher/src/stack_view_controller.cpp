extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/stack_view_controller.h>
#include <omg/bit_helper.h>

namespace Escher {

StackViewController::StackViewController(Responder * parentResponder, ViewController * rootViewController, StackView::Style style, bool extendVertically) :
    ViewController(parentResponder),
    m_view(style, extendVertically),
    m_numberOfChildren(0),
    m_isVisible(false),
    m_displayedAsModal(false),
    m_headersDisplayMask(~0)
{
  m_childrenController[m_numberOfChildren++] = rootViewController;
  rootViewController->setParentResponder(this);
}

const char * StackViewController::title() {
  ViewController * vc = m_childrenController[0];
  return vc->title();
}

ViewController * StackViewController::topViewController() {
  if (m_numberOfChildren < 1) {
    return nullptr;
  }
  return m_childrenController[m_numberOfChildren - 1];
}

void StackViewController::push(ViewController * vc) {
  assert(m_numberOfChildren < k_maxNumberOfStacks);
  /* Add the frame to the model */
  pushModel(vc);
  vc->initView();
  if (!m_isVisible) {
    return;
  }
  setupActiveViewController();
  if (m_numberOfChildren > 1) {
    m_childrenController[m_numberOfChildren - 2]->viewDidDisappear();
  }
}

void StackViewController::pop() {
  assert(m_numberOfChildren > 0);
  dismissPotentialModal();
  ViewController * vc = topViewController();
  m_numberOfChildren--;
  setupActiveViewController();
  vc->setParentResponder(nullptr);
  vc->viewDidDisappear();
  didExitPage(vc);
}

void StackViewController::popUntilDepth(int depth, bool shouldSetupTopViewController) {
  /* If final active view is meant to disappear afterward, there is no need to
   * call setupActiveViewController(), which layouts the final view.
   * For example, shouldSetupTopViewController should be set to false if push,
   * viewDidDisappear, viewWillAppear or pop are called afterward. */
  assert(depth >= 0);
  if (depth >= m_numberOfChildren) {
    return;
  }
  dismissPotentialModal();
  int numberOfFramesReleased = m_numberOfChildren - depth;
  ViewController * vc;
  for (int i = 0; i < numberOfFramesReleased; i++) {
    vc = topViewController();
    m_numberOfChildren--;
    if (shouldSetupTopViewController && i == numberOfFramesReleased - 1) {
      setupActiveViewController();
    }
    vc->setParentResponder(nullptr);
    if (i == 0) {
      vc->viewDidDisappear();
    }
    didExitPage(vc);
  }
}

void StackViewController::pushModel(ViewController * controller) {
  willOpenPage(controller);
  m_childrenController[m_numberOfChildren++] = controller;
}

void StackViewController::setupActiveView() {
  ViewController * vc = topViewController();
  if (vc) {
    ViewController::TitlesDisplay topHeaderDisplayParameter = vc->titlesDisplay();
    updateStack(topHeaderDisplayParameter);
    m_view.setContentView(vc->view());
    vc->viewWillAppear();
  }
}

void StackViewController::setupActiveViewController() {
  ViewController * vc = topViewController();
  if (vc) {
    vc->setParentResponder(this);
  }
  setupActiveView();
  Container::activeApp()->setFirstResponder(vc);
}

void StackViewController::didEnterResponderChain(Responder * previousFirstResponder) {
  m_displayedAsModal = Container::activeApp()->modalViewController()->isDisplayingModal();
}

void StackViewController::didBecomeFirstResponder() {
  ViewController * vc = topViewController();
  Container::activeApp()->setFirstResponder(vc);
}

bool StackViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_numberOfChildren > 1) {
    pop();
    return true;
  }
  return false;
}

void StackViewController::initView() {
  m_childrenController[0]->initView();
}

void StackViewController::viewWillAppear() {
  /* Load the visible controller view */
  setupActiveView();
  m_isVisible = true;
}

void StackViewController::viewDidDisappear() {
  ViewController * vc = topViewController();
  if (vc) {
    vc->viewDidDisappear();
  }
  m_isVisible = false;
  m_view.setContentView(nullptr);
}

bool StackViewController::shouldStoreHeaderOnStack(ViewController * vc, int index) {
  /* In general, the titlesDisplay controls how the stack is shown
   * only while the controller is the last on the stack. */
  return vc->title() != nullptr &&
         vc->titlesDisplay() != ViewController::TitlesDisplay::NeverDisplayOwnTitle &&
         OMG::BitHelper::bitAtIndex(m_headersDisplayMask, m_numberOfChildren - 1 - index);
}

void StackViewController::updateStack(ViewController::TitlesDisplay titleDisplay) {
  /* Update the header display mask */
  // If NeverDisplayOwnTitle, we show all other headers -> DisplayAllTitles
  m_headersDisplayMask = static_cast<uint8_t>(titleDisplay);

  /* Load the stack view */
  m_view.resetStack();
  for (int i = 0; i < m_numberOfChildren; i++) {
    ViewController * childrenVC = m_childrenController[i];
    if (shouldStoreHeaderOnStack(childrenVC, i)) {
      m_view.pushStack(m_childrenController[i]);
    }
  }
}

void StackViewController::didExitPage(ViewController * controller) const {
  Container::activeApp()->didExitPage(controller);
}

void StackViewController::willOpenPage(ViewController * controller) const {
  Container::activeApp()->willOpenPage(controller);
}

void StackViewController::dismissPotentialModal() {
  if (!m_displayedAsModal) {
    Container::activeApp()->modalViewController()->dismissPotentialModal();
  }
}

}  // namespace Escher
