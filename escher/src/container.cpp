#include <escher/container.h>
#include <ion/display.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Container::Container() :
  m_activeApp(nullptr)
{
}

void Container::switchTo(App * app) {
  m_activeApp = app;
  m_activeApp->setWindow(&m_window);
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

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg([](void * ctx){ ((Container *)ctx)->step(); }, this, 0, 1);
#else
  while (true) {
    step();
  }
#endif
}

void Container::step() {
  Ion::Events::Event event = Ion::Events::getEvent(); // This is a blocking call
  if (handleEvent(event)) {
    return;
  }
  m_activeApp->processEvent(event);
  m_window.redraw();
}
