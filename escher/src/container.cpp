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

bool Container::dispatchEvent(Ion::Events::Event event) {
  if (m_activeApp->processEvent(event)) {
    window()->redraw();
    return true;
  }
  return false;
}

void Container::run() {
  window()->setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  window()->redraw();
  RunLoop::run();
}
