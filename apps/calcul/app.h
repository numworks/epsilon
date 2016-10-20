#ifndef CALCUL_APP_H
#define CALCUL_APP_H

#include <escher.h>
#include "calcul_controller.h"

namespace Calcul {

class App : public ::App {
public:
  App();
protected:
  ViewController * rootViewController() override;
private:
  CalculController m_calculController;
};

}

#endif
