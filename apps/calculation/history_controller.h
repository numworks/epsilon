#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include <escher.h>
#include "history_view_cell.h"
#include "calculation_store.h"
#include "selectable_table_view.h"

namespace Calculation {

class App;

class HistoryController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate, public HistoryViewCellDataSource {
public:
  HistoryController(Responder * parentResponder, CalculationStore * calculationStore);
  View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void reload();
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
  void scrollToCell(int i, int j);
private:
  int storeIndex(int i) { return numberOfRows() - i - 1; }
  Shared::ExpiringPointer<Calculation> calculationAtIndex(int i);
  CalculationSelectableTableView * selectableTableView();
  HistoryViewCell * historyViewCellDidChangeSelection() override;
  constexpr static int k_maxNumberOfDisplayedRows = 5;
  CalculationSelectableTableView m_selectableTableView;
  HistoryViewCell m_calculationHistory[k_maxNumberOfDisplayedRows];
  CalculationStore * m_calculationStore;
};

}

#endif
