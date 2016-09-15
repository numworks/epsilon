#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <assert.h>
}

App::App() :
  m_magic(Magic),
  Responder(nullptr),
  m_focusedResponder(nullptr)
{
}

void App::setWindow(Window * window) {
  ViewController * controller = rootViewController();
  View * view = controller->view();
  if (m_focusedResponder == nullptr) {
    focus(controller);
  }
  assert(controller->app() == this);

  window->setContentView(view);
  view->setFrame(window->bounds());

  window->redraw();
}

void App::processEvent(Ion::Events::Event event) {
  Responder * responder = m_focusedResponder;
  bool didHandleEvent = false;
  while (responder) {
    didHandleEvent = responder->handleEvent(event);
    if (didHandleEvent) {
      break;
    }
    responder = responder->parentResponder();
  }
}

void App::focus(Responder * responder) {
  if (m_focusedResponder) {
    m_focusedResponder->setFocused(false);
  }
  m_focusedResponder = responder;
  if (m_focusedResponder) {
    m_focusedResponder->setFocused(true);
  }
}
