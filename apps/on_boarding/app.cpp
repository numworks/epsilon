#include "app.h"
#include "../apps_container.h"

namespace OnBoarding {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

App::App(Container * container, Descriptor * descriptor) :
  ::App(container, &m_languageController, descriptor),
  m_languageController(&m_modalViewController, &m_logoController, ((AppsContainer *)container)->updatePopUpController()),
  m_logoController()
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
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
