#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include "law/law_controller.h"

namespace Probability {

class App : public ::App {
public:
  App();
protected:
  ViewController * rootViewController() override;
private:
  LawController m_lawController;
  //ListController m_listController;
  //GraphController m_graphController;
  StackViewController m_stackViewController;
};

}

#endif
