#include <escher/container.h>
extern "C" {
#include <ion.h>
}

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

bool Container::handleEvent(ion_event_t event) {
  return false;
}

void Container::run() {
  m_window.setFrame(KDRect(0, 0, ION_SCREEN_WIDTH, ION_SCREEN_HEIGHT));
  m_window.redraw();

  while (true) {
    ion_event_t event = ion_get_event(); // This is a blocking call
    if (handleEvent(event)) {
      continue;
    }
    m_activeApp->processEvent(event);
    m_window.redraw();
  }
}
