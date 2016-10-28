#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "history_view_cell.h"
#include "calculation_store.h"
#include "selectable_table_view.h"

namespace Calculation {

class App;

class HistoryController : public ViewController, public ListViewDataSource, public SelectableTableViewDelegate {
public:
  HistoryController(Responder * parentResponder, CalculationStore * calculationStore);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Function * function);
  void didBecomeFirstResponder() override;
  void reload();
  int numberOfRows() override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
private:
  constexpr static int k_maxNumberOfDisplayedRows = 10;
  constexpr static KDCoordinate k_resultHeight = 12;
  HistoryViewCell m_calculationHistory[k_maxNumberOfDisplayedRows];
  CalculationSelectableTableView m_selectableTableView;
  CalculationStore * m_calculationStore;
};

}

#endif
