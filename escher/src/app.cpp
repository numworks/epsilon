#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <ion.h>
}

App::App() :
  Responder(nullptr),
  m_focusedResponder(nullptr)
{
}

void App::run() {
  Window window;
  window.setFrame(KDRect(0, 0, ION_SCREEN_WIDTH, ION_SCREEN_HEIGHT));
  View * rootView = rootViewController()->view();

  focus(rootViewController());

  window.setContentView(rootView);
  rootView->setFrame(window.bounds());

  window.redraw();

  while (true) {
    ion_event_t event = ion_get_event(); // This is a blocking call
    dispatchEvent(event);
    window.redraw();
  }
}

void App::dispatchEvent(ion_event_t event) {
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
