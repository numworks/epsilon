#include <escher/container.h>
#include <assert.h>

Container::Container() :
  RunLoop(),
  m_activeApp(nullptr)
{
}

Container::~Container() {
  if (m_activeApp) {
    delete m_activeApp;
  }
}

void Container::switchTo(App::Snapshot * snapshot) {
  if (m_activeApp && snapshot == m_activeApp->snapshot()) {
    return;
  }
  if (m_activeApp) {
    m_activeApp->willBecomeInactive();
    m_activeApp->snapshot()->pack(m_activeApp);
    m_activeApp = nullptr;
  }
  if (snapshot) {
    m_activeApp = snapshot->unpack(this);
  }
  if (m_activeApp) {
    m_activeApp->didBecomeActive(window());
    window()->redraw();
  }
}

App * Container::activeApp() {
  return m_activeApp;
}

bool Container::dispatchEvent(Ion::Events::Event event) {
  if (event == Ion::Events::TimerFire || m_activeApp->processEvent(event)) {
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
  return m_activeApp->numberOfTimers() + numberOfContainerTimers();
}

Timer * Container::timerAtIndex(int i) {
  if (i < m_activeApp->numberOfTimers()) {
    return m_activeApp->timerAtIndex(i);
  }
  return containerTimerAtIndex(i-m_activeApp->numberOfTimers());
}

int Container::numberOfContainerTimers() {
  return 0;
}

Timer * Container::containerTimerAtIndex(int i) {
  assert(false);
  return nullptr;
}
