#ifndef GRAPH_ABSCISSA_PARAM_CONTROLLER_H
#define GRAPH_ABSCISSA_PARAM_CONTROLLER_H

#include <escher.h>
#include "values_parameter_controller.h"

namespace Graph {
class AbscissaParameterController : public ViewController, public SimpleListViewDataSource {
public:
  AbscissaParameterController(Responder * parentResponder, ValuesParameterController * valuesParameterController);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  void setActiveCell(int index);
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  bool handleEnter();
  constexpr static int k_totalNumberOfCell = 3;
  ListViewCell m_deleteColumn;
  ListViewCell m_copyColumn;
  ListViewCell m_setInterval;
  ListView m_listView;
  int m_activeCell;
  ValuesParameterController * m_valuesParameterController;
};

}

#endif
