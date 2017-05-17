#include "app.h"
#include "../apps_container.h"

namespace OnBoarding {

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_languageController),
  m_languageController(&m_modalViewController, &m_logoController, ((AppsContainer *)container)->updatePopUpController()),
  m_logoController()
{
}

void App::reinitOnBoarding() {
  m_languageController.reinitOnBoarding();
}

bool App::hasTimer() {
  return firstResponder() == &m_logoController;
}

Timer * App::timer() {
  return &m_logoController;
}

}
