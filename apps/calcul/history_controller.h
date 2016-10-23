#ifndef CALCUL_HISTORY_CONTROLLER_H
#define CALCUL_HISTORY_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "history_view_cell.h"
#include "calcul_store.h"

namespace Calcul {

class App;

class HistoryController : public ViewController, ListViewDataSource {
public:
  HistoryController(Responder * parentResponder, CalculStore * calculStore);

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
  constexpr static int k_defaultCalculCellWidth = 320;
  constexpr static int k_resultWidth = 7*14;
  HistoryViewCell m_calculHistory[k_maxNumberOfDisplayedRows];
  ListView m_listView;
  int m_activeCell;
  CalculStore * m_calculStore;
};

}

#endif
