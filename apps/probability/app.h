#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include "law/law_controller.h"
#include "parameters/parameters_controller.h"

namespace Probability {

class App : public ::App {
public:
  enum class Law {
    Normal,
    Student
  };
  App();
  void setLaw(Law l);
protected:
  ViewController * rootViewController() override;
private:
  LawController m_lawController;
  ParametersController m_parametersController;
  StackViewController m_stackViewController;
};

}

#endif
