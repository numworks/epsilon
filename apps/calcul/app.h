#ifndef CALCUL_APP_H
#define CALCUL_APP_H

#include "edit_expression_controller.h"
#include "history_controller.h"
#include <escher.h>

namespace Calcul {

class App : public ::App {
public:
  App(Context * context);
  Context * globalContext();
protected:
  ViewController * rootViewController() override;
private:
  Context * m_globalContext;
  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
