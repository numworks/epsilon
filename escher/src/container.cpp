#include <escher/container.h>
#include <ion/display.h>

Container::Container() :
  m_activeApp(nullptr)
{
}

void Container::switchTo(App * app) {
  m_activeApp = app;
  m_activeApp->setWindow(&m_window);
  m_window.redraw();
}

App * Container::activeApp() {
  return m_activeApp;
}

bool Container::handleEvent(Ion::Events::Event event) {
  return false;
}

void Container::run() {
  m_window.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  m_window.redraw();

  while (true) {
    Ion::Events::Event event = Ion::Events::getEvent(); // This is a blocking call
    if (handleEvent(event)) {
      continue;
    }
    m_activeApp->processEvent(event);
    m_window.redraw();
  }
}
