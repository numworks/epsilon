#include "model.h"

#include <apps/global_preferences.h>
#include <apps/i18n.h>

namespace Regression {

static bool UseLinearMxpbForm() {
  return GlobalPreferences::SharedGlobalPreferences()->regressionAppVariant() ==
         CountryPreferences::RegressionApp::Variant1;
}

I18n::Message Model::name() const {
  switch (m_type) {
    case Type::None:
      // Used in regression model menus when no regression is selected
      return I18n::Message::Default;
    case Type::LinearAxpb:
    case Type::LinearApbx:
      return I18n::Message::Linear;
    case Type::Proportional:
      return I18n::Message::Proportional;
    case Type::Quadratic:
      return I18n::Message::Quadratic;
    case Type::Cubic:
      return I18n::Message::Cubic;
    case Type::Quartic:
      return I18n::Message::Quartic;
    case Type::Logarithmic:
      return I18n::Message::Logarithmic;
    case Type::ExponentialAebx:
    case Type::ExponentialAbx:
      return I18n::Message::ExponentialRegression;
    case Type::Power:
      return I18n::Message::Power;
    case Type::Trigonometric:
      return I18n::Message::Trigonometrical;
    case Type::Logistic:
      return I18n::Message::Logistic;
    case Type::Median:
      return I18n::Message::MedianRegression;
  }
}

I18n::Message Model::formulaMessage() const {
  switch (m_type) {
    case Type::None:
      assert(false);
    case Type::LinearAxpb:
      return UseLinearMxpbForm() ? I18n::Message::LinearMxpbRegressionFormula
                                 : I18n::Message::LinearRegressionFormula;
    case Type::LinearApbx:
      return I18n::Message::LinearApbxRegressionFormula;
    case Type::Proportional:
      return I18n::Message::ProportionalRegressionFormula;
    case Type::Quadratic:
      return I18n::Message::QuadraticRegressionFormula;
    case Type::Cubic:
      return I18n::Message::CubicRegressionFormula;
    case Type::Quartic:
      return I18n::Message::QuarticRegressionFormula;
    case Type::Logarithmic:
      return I18n::Message::LogarithmicRegressionFormula;
    case Type::ExponentialAebx:
      return I18n::Message::ExponentialAebxRegressionFormula;
    case Type::ExponentialAbx:
      return I18n::Message::ExponentialAbxRegressionFormula;
    case Type::Power:
      return I18n::Message::PowerRegressionFormula;
    case Type::Trigonometric:
      return I18n::Message::TrigonometricRegressionFormula;
    case Type::Logistic:
      return I18n::Message::LogisticRegressionFormula;
    case Type::Median:
      assert(false);
  }
}

}  // namespace Regression
