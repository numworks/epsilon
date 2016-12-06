#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include "law_controller.h"
#include "evaluate_context.h"
#include "../expression_text_field_delegate.h"

namespace Probability {

class App : public ::App, public ExpressionTextFieldDelegate {
public:
  enum class Law {
    Normal,
    Student
  };
  App(Container * container, Context * context);
  Context * evaluateContext() override;
private:
  EvaluateContext m_evaluateContext;
  LawController m_lawController;
  StackViewController m_stackViewController;
};

}

#endif
