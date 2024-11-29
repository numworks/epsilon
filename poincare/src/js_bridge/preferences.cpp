#include "preferences.h"

#include <emscripten/bind.h>
#include <poincare/preferences.h>

#include <string>
using namespace emscripten;

namespace Poincare::JSBridge {

EMSCRIPTEN_BINDINGS(preferences) {
  enum_<Preferences::ComplexFormat>("ComplexFormat")
      .value("Real", Preferences::ComplexFormat::Real)
      .value("Cartesian", Preferences::ComplexFormat::Cartesian)
      .value("Polar", Preferences::ComplexFormat::Polar);
  enum_<Preferences::AngleUnit>("AngleUnit")
      .value("Radian", Preferences::AngleUnit::Radian)
      .value("Degree", Preferences::AngleUnit::Degree)
      .value("Gradian", Preferences::AngleUnit::Gradian);
  enum_<Preferences::UnitFormat>("UnitFormat")
      .value("Metric", Preferences::UnitFormat::Metric)
      .value("Imperial", Preferences::UnitFormat::Imperial);
  enum_<Preferences::PrintFloatMode>("PrintFloatMode")
      .value("Decimal", Preferences::PrintFloatMode::Decimal)
      .value("Scientific", Preferences::PrintFloatMode::Scientific)
      .value("Engineering", Preferences::PrintFloatMode::Engineering);

  value_object<ReductionPreferences>("ReductionPreferences")
      .field("complexFormat", &ReductionPreferences::complexFormat)
      .field("angleUnit", &ReductionPreferences::angleUnit)
      .field("unitFormat", &ReductionPreferences::unitFormat);
  value_object<PrintFloatPreferences>("PrintFloatPreferences")
      .field("printFloatMode", &PrintFloatPreferences::printFloatMode)
      .field("numberOfSignificantDigits",
             &PrintFloatPreferences::numberOfSignificantDigits);
}

}  // namespace Poincare::JSBridge
