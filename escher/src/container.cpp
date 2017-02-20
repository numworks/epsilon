#include <escher/container.h>
#include <ion.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Container::Container() :
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

bool Container::handleEvent(Ion::Events::Event event) {
  return false;
}

void Container::run() {
  window()->setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  window()->redraw();

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
#if ESCHER_LOG_EVENTS
  char message[2] = { (char)event.id(), 0};
  Ion::Log::print(message);
#endif

  if (event == Ion::Events::None) {
    return;
  }
  if (handleEvent(event)) {
    return;
  }
  m_activeApp->processEvent(event);
  window()->redraw();
}
