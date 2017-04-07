#include "resettable_app.h"

namespace Shared {

ResettableApp::ResettableApp(Container * container, ViewController * rootViewController, I18n::Message name, I18n::Message upperName, const Image * icon) :
  ::App(container, rootViewController, name, upperName, icon, I18n::Message::Warning)
{
}

void ResettableApp::reset() {
}

}
