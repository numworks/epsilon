#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <assert.h>
}

App::App(ViewController * rootViewController, const char * name, const Image * icon) :
  Responder(nullptr),
  m_magic(Magic),
  m_modalViewController(ModalViewController(this, rootViewController)),
  m_firstResponder(nullptr),
  m_name(name),
  m_icon(icon)
{
}

void App::setWindow(Window * window) {
  View * view = m_modalViewController.view();
  if (m_firstResponder == nullptr) {
    setFirstResponder(&m_modalViewController);
  }
  assert(m_modalViewController.app() == this);

  window->setContentView(view);
  view->setFrame(window->bounds());

  window->redraw();
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

const Image * App::icon() {
  return m_icon;
}

void App::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment) {
  m_modalViewController.displayModalViewController(vc, verticalAlignment, horizontalAlignment);
}

void App::dismissModalViewController() {
  m_modalViewController.dismissModalViewController();
}
