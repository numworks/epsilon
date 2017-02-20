#include <escher/container.h>
#include <ion.h>

Container::Container() :
  RunLoop(),
  m_activeApp(nullptr)
{
}

void Container::switchTo(App * app) {
  if (m_activeApp == app) {
    return;
  }
  if (m_activeApp) {
    m_activeApp->willBecomeInactive();
  }
  m_activeApp = app;
  if (m_activeApp) {
    m_activeApp->didBecomeActive(window());
  }
}

App * Container::activeApp() {
  return m_activeApp;
}

void Container::dispatchEvent(Ion::Events::Event event) {
#if ESCHER_LOG_EVENTS_BINARY
  char message[2] = { (char)event.id(), 0};
  ion_log_string(message);
#endif
#if ESCHER_LOG_EVENTS_NAME
  const char * name = event.name();
  if (name == nullptr) {
    name = "UNDEFINED";
  }
  ion_log_string("Ion::Events::");
  ion_log_string(name);
  ion_log_string("\n");
#endif
  m_activeApp->processEvent(event);
  window()->redraw();
}

void Container::run() {
  window()->setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  window()->redraw();
  RunLoop::run();
}
