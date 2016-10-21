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

  void setActiveCell(int index);
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(View * cell, int index) override;

private:
  constexpr static int k_maxNumberOfDisplayedRows = 10;
  HistoryViewCell m_calculHistory[k_maxNumberOfDisplayedRows];
  ListView m_listView;
  int m_activeCell;
  CalculStore * m_calculStore;
};

}

#endif
