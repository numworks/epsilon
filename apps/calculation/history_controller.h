#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "history_view_cell.h"
#include "calculation_store.h"

namespace Calculation {

class App;

class HistoryController : public ViewController, ListViewDataSource {
public:
  HistoryController(Responder * parentResponder, CalculationStore * calculationStore);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Function * function);
  void didBecomeFirstResponder() override;
  void reload();
  void setActiveCell(int index);
  int numberOfRows() override;
  View * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(View * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;

private:
  constexpr static int k_maxNumberOfDisplayedRows = 10;
  constexpr static int k_defaultCalculationCellWidth = 320;
  constexpr static int k_resultWidth = 7*14;
  HistoryViewCell m_calculationHistory[k_maxNumberOfDisplayedRows];
  ListView m_listView;
  int m_activeCell;
  CalculationStore * m_calculationStore;
};

}

#endif
