#ifndef SHARED_RESETTABLE_APP_H
#define SHARED_RESETTABLE_APP_H

#include <escher.h>
#include "../i18n.h"

namespace Shared {

class ResettableApp : public ::App {
public:
  ResettableApp(Container * container, ViewController * rootViewController, I18n::Message name = (I18n::Message)0, I18n::Message upperName = (I18n::Message)0, const Image * icon = nullptr);
  virtual void reset();
};

}

#endif
