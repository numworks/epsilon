#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include <poincare.h>
#include "law_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Probability {

class App : public Shared::TextFieldDelegateApp {
public:
  App(Container * container);
private:
  LawController m_lawController;
  StackViewController m_stackViewController;
};

}

#endif
