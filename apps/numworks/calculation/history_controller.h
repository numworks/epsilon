#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "history_view_cell.h"
#include "calculation_store.h"
#include "selectable_table_view.h"

namespace Calculation {

class App;

class HistoryController : public DynamicViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  HistoryController(Responder * parentResponder, CalculationStore * calculationStore);

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void reload();
  int numberOfRows() override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
  void scrollToCell(int i, int j);
  View * loadView() override;
  void unloadView(View * view) override;
private:
  CalculationSelectableTableView * selectableTableView();
  constexpr static int k_maxNumberOfDisplayedRows = 5;
  HistoryViewCell * m_calculationHistory[k_maxNumberOfDisplayedRows];
  CalculationStore * m_calculationStore;
};

}

#endif
