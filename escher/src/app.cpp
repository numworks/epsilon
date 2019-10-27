#include <escher/app.h>
#include <escher/window.h>
#include <poincare/tree_pool.h>
extern "C" {
#include <assert.h>
}

I18n::Message App::Descriptor::name() {
  return (I18n::Message)0;
}

I18n::Message App::Descriptor::upperName() {
  return (I18n::Message)0;
}

const Image * App::Descriptor::icon() {
  return nullptr;
}

void App::Snapshot::pack(App * app) {
  tidy();
  app->~App();
  assert(Poincare::TreePool::sharedPool()->numberOfNodes() == 0);
}

void App::Snapshot::reset() {
}

void App::Snapshot::tidy() {
}

App::App(Snapshot * snapshot, ViewController * rootViewController, I18n::Message warningMessage) :
  Responder(nullptr),
  m_modalViewController(this, rootViewController),
  m_firstResponder(nullptr),
  m_snapshot(snapshot),
  m_warningController(this, warningMessage)
{
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

Responder * App::firstResponder() {
  return m_firstResponder;
}

void App::setFirstResponder(Responder * responder) {
  Responder * previousResponder = m_firstResponder;
  m_firstResponder = responder;
  if (previousResponder) {
    Responder * commonAncestor = previousResponder->commonAncestorWith(m_firstResponder);
    Responder * leafResponder = previousResponder;
    while (leafResponder != commonAncestor) {
      leafResponder->willExitResponderChain(m_firstResponder);
      leafResponder = leafResponder->parentResponder();
    }
    previousResponder->willResignFirstResponder();
  }
  if (m_firstResponder) {
    Responder * commonAncestor = m_firstResponder->commonAncestorWith(previousResponder);
    Responder * leafResponder = m_firstResponder;
    while (leafResponder != commonAncestor) {
      leafResponder->didEnterResponderChain(previousResponder);
      leafResponder = leafResponder->parentResponder();
    }
    m_firstResponder->didBecomeFirstResponder();
  }
}

void App::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin) {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  m_modalViewController.displayModalViewController(vc, verticalAlignment, horizontalAlignment, topMargin, leftMargin, bottomMargin, rightMargin);
}

void App::dismissModalViewController() {
  m_modalViewController.dismissModalViewController();
}

void App::displayWarning(I18n::Message warningMessage1, I18n::Message warningMessage2, bool specialExitKeys) {
  m_warningController.setLabel(warningMessage1, warningMessage2, specialExitKeys);
  displayModalViewController(&m_warningController, 0.5f, 0.5f);
}

void App::didBecomeActive(Window * window) {
  View * view = m_modalViewController.view();
  m_modalViewController.initView();
  window->setContentView(view);
  m_modalViewController.viewWillAppear();
  setFirstResponder(&m_modalViewController);
}

void App::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    dismissModalViewController();
  }
  setFirstResponder(nullptr);
  m_modalViewController.viewDidDisappear();
}

View * App::modalView() {
  return m_modalViewController.view();
}

int App::numberOfTimers() {
  return 0;
}

Timer * App::timerAtIndex(int i) {
  assert(false);
  return nullptr;
}
