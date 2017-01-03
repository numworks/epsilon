#ifndef REGRESSION_INITIALISATION_PARAMETER_CONTROLLER_H
#define REGRESSION_INITIALISATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "data.h"

namespace Regression {

class InitialisationParameterController : public ViewController, public SimpleListViewDataSource {
public:
  InitialisationParameterController(Responder * parentResponder, Data * data);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCells = 3;
  MenuListCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  Data * m_data;
};

}

#endif
