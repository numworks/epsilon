#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <escher/i18n.h>
#include <poincare/src/regression/regression.h>

namespace Regression {

class Store;

class Model {
  using Type = Poincare::Regression::Regression::Type;

 public:
  I18n::Message formulaMessage() const;
  I18n::Message name() const;

 private:
  Type m_type;
};

}  // namespace Regression

#endif
