#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <assert.h>
}

App::App(Container * container, ViewController * rootViewController, I18n::Message name, I18n::Message upperName, const Image * icon, I18n::Message warningMessage) :
  Responder(nullptr),
  m_magic(Magic),
  m_modalViewController(ModalViewController(this, rootViewController)),
  m_container(container),
  m_firstResponder(nullptr),
  m_warningController(WarningController(this, warningMessage)),
  m_name(name),
  m_upperName(upperName),
  m_icon(icon)
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

I18n::Message App::name() {
  return m_name;
}

I18n::Message App::upperName() {
  return m_upperName;
}

const Image * App::icon() {
  return m_icon;
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

void App::displayWarning(I18n::Message warningMessage) {
  m_warningController.setLabel(warningMessage);
  m_modalViewController.displayModalViewController(&m_warningController, 0.5f, 0.5f);
}

const Container * App::container() const {
  return m_container;
}

void App::didBecomeActive(Window * window) {
  m_modalViewController.loadView();
  View * view = m_modalViewController.view();
  assert(m_modalViewController.app() == this);
  window->setContentView(view);
  m_modalViewController.viewWillAppear();
  setFirstResponder(&m_modalViewController);
  window->redraw();
}

void App::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    dismissModalViewController();
  }
  setFirstResponder(nullptr);
  m_modalViewController.viewDidDisappear();
  m_modalViewController.unloadView();
}
