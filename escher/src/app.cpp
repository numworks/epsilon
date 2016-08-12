#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <ion.h>
#include <assert.h>
}

App::App() :
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

void App::processEvent(ion_event_t event) {
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
