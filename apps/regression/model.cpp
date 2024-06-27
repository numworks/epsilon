#include "model.h"

#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <omg/unreachable.h>

#include "store.h"

namespace Regression {

double StoreToSeries::getX(int i) const { return m_store->get(m_series, 0, i); }

double StoreToSeries::getY(int i) const { return m_store->get(m_series, 1, i); }

int StoreToSeries::numberOfPairs() const {
  return m_store->numberOfPairsOfSeries(m_series);
}

bool Model::useLinearMxpbForm() const {
  return (m_type == Type::LinearAxpb || m_type == Type::Median) &&
         GlobalPreferences::SharedGlobalPreferences()->regressionAppVariant() ==
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
  OMG::unreachable();
}

}  // namespace Regression
