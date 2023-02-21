#include <assert.h>
#include <escher/container.h>

namespace Escher {

Container::Container() : RunLoop() {}

// Initialize private static member
App* Container::s_activeApp = nullptr;

Container::~Container() {
  if (s_activeApp) {
    s_activeApp->~App();
  }
}

void Container::switchToBuiltinApp(App::Snapshot* snapshot) {
  if (s_activeApp && snapshot == s_activeApp->snapshot()) {
    return;
  }
  if (s_activeApp) {
    s_activeApp->willBecomeInactive();
    s_activeApp->snapshot()->pack(s_activeApp);
    s_activeApp = nullptr;
  }
  if (snapshot) {
    s_activeApp = snapshot->unpack(this);
  }
  if (s_activeApp) {
    s_activeApp->didBecomeActive(window());
    window()->redraw();
  }
}

bool Container::dispatchEvent(Ion::Events::Event event) {
  if (event == Ion::Events::TimerFire) {
    window()->redraw();
    return true;
  }
  if (s_activeApp->processEvent(event)) {
    window()->redraw();
    return true;
  }
  return false;
}

void Container::run() {
  window()->redraw();
  RunLoop::run();
}

int Container::numberOfTimers() {
  return s_activeApp->numberOfTimers() + numberOfContainerTimers();
}

Timer* Container::timerAtIndex(int i) {
  if (i < s_activeApp->numberOfTimers()) {
    return s_activeApp->timerAtIndex(i);
  }
  return containerTimerAtIndex(i - s_activeApp->numberOfTimers());
}

int Container::numberOfContainerTimers() { return 0; }

Timer* Container::containerTimerAtIndex(int i) {
  assert(false);
  return nullptr;
}

}  // namespace Escher
