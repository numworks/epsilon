#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <assert.h>
}

App::App(Container * container, ViewController * rootViewController, const char * name, const char * upperName, const Image * icon) :
  Responder(nullptr),
  m_magic(Magic),
  m_modalViewController(ModalViewController(this, rootViewController)),
  m_container(container),
  m_firstResponder(nullptr),
  m_warningController(WarningController(this)),
  m_name(name),
  m_upperName(upperName),
  m_icon(icon)
{
}

void App::processEvent(Ion::Events::Event event) {
  Responder * responder = m_firstResponder;
  bool didHandleEvent = false;
  while (responder) {
    didHandleEvent = responder->handleEvent(event);
    if (didHandleEvent) {
      break;
    }
    responder = responder->parentResponder();
  }
}

Responder * App::firstResponder() {
  return m_firstResponder;
}

void App::setFirstResponder(Responder * responder) {
  if (m_firstResponder) {
    m_firstResponder->didResignFirstResponder();
  }
  m_firstResponder = responder;
  if (m_firstResponder) {
    m_firstResponder->didBecomeFirstResponder();
  }
}

const char * App::name() {
  return m_name;
}

const char * App::upperName() {
  return m_upperName;
}

const Image * App::icon() {
  return m_icon;
}

void App::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin) {
  m_modalViewController.displayModalViewController(vc, verticalAlignment, horizontalAlignment, topMargin, leftMargin, bottomMargin, rightMargin);
}

void App::dismissModalViewController() {
  m_modalViewController.dismissModalViewController();
}

void App::displayWarning(const char * warningMessage) {
  m_warningController.setLabel(warningMessage);
  m_modalViewController.displayModalViewController(&m_warningController, 0.5f, 0.5f);
}

const Container * App::container() const {
  return m_container;
}

void App::didBecomeActive(Window * window) {
  View * view = m_modalViewController.view();
  assert(m_modalViewController.app() == this);
  window->setContentView(view);
  if (m_firstResponder == nullptr) {
    setFirstResponder(&m_modalViewController);
  }
  window->redraw();
  m_modalViewController.viewWillAppear();
}

void App::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    dismissModalViewController();
  }
  m_modalViewController.viewWillDisappear();
}
