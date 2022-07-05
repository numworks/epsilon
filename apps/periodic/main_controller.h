#ifndef PERIODIC_MAIN_CONTROLLER_H
#define PERIODIC_MAIN_CONTROLLER_H

#include "elements_view.h"
#include <escher/view_controller.h>

namespace Periodic {

class MainController : public Escher::ViewController {
public:
  MainController(Escher::Responder * parentResponder, ElementsViewDataSource * dataSource) : ViewController(parentResponder), m_elementsView(dataSource) {}

  Escher::View * view() override { return &m_elementsView; }

private:
  ElementsView m_elementsView;
};

}

#endif

