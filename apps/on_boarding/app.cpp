#include "app.h"
#include <assert.h>

namespace OnBoarding {

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_languageController),
  m_languageController(&m_modalViewController, &m_logoController),
  m_logoController()
{
}

int App::numberOfTimers() {
  return firstResponder() == &m_logoController;
}

Timer * App::timerAtIndex(int i) {
  assert(i == 0);
  return &m_logoController;
}

bool App::processEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Home) {
    return true;
  }
  if (e == Ion::Events::OnOff) {
    m_languageController.reinitOnBoarding();
  }
  return ::App::processEvent(e);
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  m_languageController.reinitOnBoarding();
}

}
