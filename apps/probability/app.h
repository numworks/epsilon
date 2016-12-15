#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include <poincare.h>
#include "law_controller.h"
#include "../expression_text_field_delegate.h"

namespace Probability {

class App : public ::App, public ExpressionTextFieldDelegate {
public:
  enum class Law {
    Normal,
    Student
  };
  App(Container * container, Context * context);
  Context * localContext() override;
private:
  LawController m_lawController;
  StackViewController m_stackViewController;
};

}

#endif
