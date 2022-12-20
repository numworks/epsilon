#include <escher/app.h>
#include <escher/window.h>
#include <poincare/tree_pool.h>
extern "C" {
#include <assert.h>
}

namespace Escher {

void App::Snapshot::pack(App * app) {
  tidy();
  app->~App();
  assert(Poincare::TreePool::sharedPool()->numberOfNodes() == 0);
}

bool App::processEvent(Ion::Events::Event event) {
  Responder * responder = m_firstResponder;
  bool didHandleEvent = false;
  while (responder) {
    didHandleEvent = responder->handleEvent(event);
    if (didHandleEvent) {
      return true;
    }
    responder = responder->parentResponder();
  }
  return false;
}

void App::setFirstResponder(Responder * responder) {
  /* This flag is used only in DEBUG to ensure that didEnterResponderChain do
   * not call setFirstResponder. */
#ifndef NDEBUG
  static bool preventRecursion = false;
  assert(!preventRecursion);
#endif
  if (m_firstResponder == responder) {
    return;
  }
  Responder * previousResponder = m_firstResponder;
  m_firstResponder = responder;
  if (previousResponder) {
    Responder * commonAncestor = previousResponder->commonAncestorWith(m_firstResponder);
    Responder * leafResponder = previousResponder;
#ifndef NDEBUG
    preventRecursion = true;
#endif
    while (leafResponder != commonAncestor) {
      leafResponder->willExitResponderChain(m_firstResponder);
      leafResponder = leafResponder->parentResponder();
    }
#ifndef NDEBUG
    preventRecursion = false;
#endif
    previousResponder->willResignFirstResponder();
  }
  if (m_firstResponder) {
    constexpr int k_maxNumberOfResponders = 32;
    Responder * responderStack[k_maxNumberOfResponders];
    int index = 0;
    Responder * commonAncestor = m_firstResponder->commonAncestorWith(previousResponder);
    Responder * leafResponder = m_firstResponder;
#ifndef NDEBUG
    preventRecursion = true;
#endif
    while (leafResponder != commonAncestor) {
      assert(index < k_maxNumberOfResponders);
      responderStack[index++] = leafResponder;
      leafResponder = leafResponder->parentResponder();
    }
    for(index--; index >= 0; index--) {
      responderStack[index]->didEnterResponderChain(previousResponder);
    }
#ifndef NDEBUG
    preventRecursion = false;
#endif
    m_firstResponder->didBecomeFirstResponder();
  }
}

void App::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin, bool growingOnly) {
  m_modalViewController.dismissPotentialModal();
  m_modalViewController.displayModalViewController(vc, verticalAlignment, horizontalAlignment, topMargin, leftMargin, bottomMargin, rightMargin, growingOnly);
}

void App::displayWarning(I18n::Message warningMessage1, I18n::Message warningMessage2, bool specialExitKeys) {
  m_warningController.setLabel(warningMessage1, warningMessage2, specialExitKeys);
  displayModalViewController(&m_warningController, KDContext::k_alignCenter, KDContext::k_alignCenter);
}

void App::didBecomeActive(Window * window) {
  View * view = m_modalViewController.view();
  m_modalViewController.initView();
  window->setContentView(view);
  m_modalViewController.viewWillAppear();
  setFirstResponder(&m_modalViewController);
}

void App::willBecomeInactive() {
  setFirstResponder(nullptr);
  m_modalViewController.viewDidDisappear();
}

}
