#ifndef ESCHER_PREFERENCES_H
#define ESCHER_PREFERENCES_H

#include <poincare/preferences.h>

namespace Escher {

namespace LayoutPreferences {

enum class LogarithmKeyEvent : char { Default, WithBaseTen };

class Interface {
 public:
  virtual bool linearMode() const = 0;
  virtual LogarithmKeyEvent logarithmKeyEvent() const = 0;
  virtual Poincare::Preferences::PrintFloatMode displayMode() const = 0;
  bool operator==(const Interface&) const = default;
};

}  // namespace LayoutPreferences

extern const LayoutPreferences::Interface* SharedPreferences;

}  // namespace Escher
#endif
