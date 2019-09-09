#ifndef REGRESSION_INITIALISATION_PARAMETER_CONTROLLER_H
#define REGRESSION_INITIALISATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include <apps/i18n.h>

namespace Regression {

class InitialisationParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  InitialisationParameterController(Responder * parentResponder, Store * store);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCells = 3;
  MessageTableCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  Store * m_store;
};

}

#endif
