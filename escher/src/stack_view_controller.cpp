extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/stack_view_controller.h>
#include <omg/bit_helper.h>

namespace Escher {

StackViewController::StackViewController(
    Responder* parentResponder, StackView::Style style, bool extendVertically,
    OMG::AbstractStack<StackHeaderView>* headerViewStack)
    : ViewController(parentResponder),
      m_view(style, extendVertically, headerViewStack),
      m_size(1),
      m_isVisible(false),
      m_displayedAsModal(false),
      m_headersDisplayMask(~0) {}

const char* StackViewController::title() const {
  const ViewController* vc = stackSlot(0);
  return vc->title();
}

ViewController* StackViewController::topViewController() {
  if (m_size < 1) {
    return nullptr;
  }
  return stackSlot(m_size - 1);
}

const ViewController* StackViewController::topViewController() const {
  if (m_size < 1) {
    return nullptr;
  }
  return stackSlot(m_size - 1);
}

void StackViewController::push(ViewController* vc) {
  /* Add the frame to the model */
  pushModel(vc);
  if (!m_isVisible) {
    return;
  }
  setupActiveViewController();
  if (m_size > 1) {
    stackSlot(m_size - 2)->viewDidDisappear();
  }
}

void StackViewController::pop() {
  assert(m_size > 0);
  dismissPotentialModal();
  ViewController* vc = topViewController();
  m_size--;
  setupActiveViewController();
  vc->setParentResponder(nullptr);
  vc->viewDidDisappear();
  didExitPage(vc);
}

void StackViewController::popUntilDepth(int depth,
                                        bool shouldSetupTopViewController) {
  /* If final active view is meant to disappear afterward, there is no need to
   * call setupActiveViewController(), which layouts the final view.
   * For example, shouldSetupTopViewController should be set to false if push,
   * viewDidDisappear, viewWillAppear or pop are called afterward. */
  assert(depth >= 0);
  if (depth >= m_size) {
    return;
  }
  dismissPotentialModal();
  int numberOfFramesReleased = m_size - depth;
  ViewController* vc;
  for (int i = 0; i < numberOfFramesReleased; i++) {
    vc = topViewController();
    m_size--;
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

void StackViewController::pushModel(ViewController* controller) {
  willOpenPage(controller);
  setStackSlot(m_size++, controller);
}

void StackViewController::setupActiveView() {
  ViewController* vc = topViewController();
  if (vc) {
    vc->initView();
    ViewController::TitlesDisplay topHeaderDisplayParameter =
        vc->titlesDisplay();
    updateStack(topHeaderDisplayParameter);
    m_view.setContentView(vc->view());
    vc->viewWillAppear();
  }
}

void StackViewController::setupActiveViewController() {
  ViewController* vc = topViewController();
  if (vc) {
    vc->setParentResponder(this);
  }
  setupActiveView();
  App::app()->setFirstResponder(vc);
}

void StackViewController::didEnterResponderChain(
    Responder* previousFirstResponder) {
  m_displayedAsModal = App::app()->modalViewController()->isDisplayingModal();
}

void StackViewController::didBecomeFirstResponder() {
  ViewController* vc = topViewController();
  App::app()->setFirstResponder(vc);
}

bool StackViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_size > 1) {
    pop();
    return true;
  }
  return false;
}

void StackViewController::initView() { stackSlot(0)->initView(); }

void StackViewController::viewWillAppear() {
  /* Load the visible controller view */
  setupActiveView();
  m_isVisible = true;
}

void StackViewController::viewDidDisappear() {
  ViewController* vc = topViewController();
  if (vc) {
    vc->viewDidDisappear();
  }
  m_isVisible = false;
  m_view.setContentView(nullptr);
}

bool StackViewController::shouldStoreHeaderOnStack(ViewController* vc,
                                                   int index) {
  /* In general, the titlesDisplay controls how the stack is shown
   * only while the controller is the last on the stack. */
  return vc->title() != nullptr &&
         vc->titlesDisplay() !=
             ViewController::TitlesDisplay::NeverDisplayOwnTitle &&
         OMG::BitHelper::bitAtIndex(m_headersDisplayMask, m_size - 1 - index);
}

void StackViewController::updateStack(
    ViewController::TitlesDisplay titleDisplay) {
  /* Update the header display mask */
  // If NeverDisplayOwnTitle, we show all other headers -> DisplayAllTitles
  m_headersDisplayMask = static_cast<StackView::Mask>(titleDisplay);

  /* Load the stack view */
  m_view.resetStack();
  for (int i = 0; i < m_size; i++) {
    ViewController* childrenVC = stackSlot(i);
    if (shouldStoreHeaderOnStack(childrenVC, i)) {
      m_view.pushStack(stackSlot(i));
    }
  }
}

void StackViewController::didExitPage(ViewController* controller) const {
  App::app()->didExitPage(controller);
}

void StackViewController::willOpenPage(ViewController* controller) const {
  App::app()->willOpenPage(controller);
}

void StackViewController::dismissPotentialModal() {
  if (!m_displayedAsModal) {
    App::app()->modalViewController()->dismissPotentialModal();
  }
}

}  // namespace Escher
