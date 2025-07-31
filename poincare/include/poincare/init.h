#ifndef POINCARE_INIT_H
#define POINCARE_INIT_H

#include "preferences.h"

namespace Poincare {

void Init(Preferences::PartialInterface* prefs = nullptr);
void Shutdown();

}  // namespace Poincare

#endif
