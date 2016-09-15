#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <assert.h>
}

App::App() :
  Responder(nullptr),
  m_magic(Magic),
  m_firstResponder(nullptr)
{
}

void App::setWindow(Window * window) {
  ViewController * controller = rootViewController();
  View * view = controller->view();
  if (m_firstResponder == nullptr) {
    setFirstResponder(controller);
  }
  assert(controller->app() == this);

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
