#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include "law_controller.h"

namespace Probability {

class App : public ::App {
public:
  enum class Law {
    Normal,
    Student
  };
  App(Container * container);
private:
  LawController m_lawController;
  StackViewController m_stackViewController;
};

}

#endif
