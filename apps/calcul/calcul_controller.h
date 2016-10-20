#ifndef CALCUL_CALCUL_CONTROLLER_H
#define CALCUL_CALCUL_CONTROLLER_H

#include <escher.h>
#include <poincare.h>

namespace Calcul {

class CalculController : public ViewController {
public:
  CalculController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;

private:
  SolidColorView m_solidColorView;
};

}

#endif
