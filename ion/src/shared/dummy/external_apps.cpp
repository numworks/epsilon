#include <ion/external_apps.h>

namespace Ion {
namespace ExternalApps {

App::App(uint8_t *) {}

const uint32_t App::APILevel() const {
  return 0;
}

const char * App::name() const {
  return "";
}

uint32_t App::iconSize() const {
  return 0;
}

const uint8_t * App::iconData() const {
  return nullptr;
}

void * App::entryPoint() const {
  return nullptr;
}

void App::eraseMagicCode() {}

AppIterator& AppIterator::operator++() {
  return *this;
}

AppIterator Apps::begin() const {
  return AppIterator(nullptr);
}

void setVisible() {}

int numberOfApps() {
  return 0;
}

void deleteApps() {}

}
}
