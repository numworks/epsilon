#ifndef GRAPH_ABSCISSA_PARAM_CONTROLLER_H
#define GRAPH_ABSCISSA_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval_parameter_controller.h"

namespace Graph {
class AbscissaParameterController : public ViewController, public SimpleListViewDataSource {
public:
  AbscissaParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  MenuListCell m_deleteColumn;
  ChevronMenuListCell m_copyColumn;
  ChevronMenuListCell m_setInterval;
  SelectableTableView m_selectableTableView;
  IntervalParameterController * m_intervalParameterController;
};

}

#endif
