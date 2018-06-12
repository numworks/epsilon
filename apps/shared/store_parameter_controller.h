#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher.h>
#include "double_pair_store.h"
#include "../i18n.h"

namespace Shared {

class StoreController;

class StoreParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StoreParameterController(Responder * parentResponder, DoublePairStore * store, StoreController * storeController);
  void selectXColumn(bool xColumnSelected) { m_xColumnSelected = xColumnSelected; }
  void selectSeries(int series) { m_series = series; }
  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override { return k_totalNumberOfCell; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override { return k_totalNumberOfCell; }
private:
#if COPY_IMPORT_LIST
  constexpr static int k_totalNumberOfCell = 4;
  MessageTableCellWithChevron m_copyColumn;
  MessageTableCellWithChevron m_importList;
#else
  constexpr static int k_totalNumberOfCell = 2;
#endif
  MessageTableCell m_deleteColumn;
  MessageTableCell m_fillWithFormula;
  SelectableTableView m_selectableTableView;
  DoublePairStore * m_store;
  StoreController * m_storeController;
  bool m_xColumnSelected;
  int m_series;
};

}

#endif
