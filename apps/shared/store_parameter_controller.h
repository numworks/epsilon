#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher.h>
#include "double_pair_store.h"
#include <apps/i18n.h>

namespace Shared {

class StoreController;

class StoreParameterController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  StoreParameterController(Responder * parentResponder, DoublePairStore * store, StoreController * storeController);
  void selectXColumn(bool xColumnSelected) { m_xColumnSelected = xColumnSelected; }
  void selectSeries(int series) { m_series = series; }
  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  KDCoordinate rowHeight(int j) override { return Metric::ParameterCellHeight; }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override {
    assert(type == k_standardCellType);
    return k_totalNumberOfCell;
  }
  int typeAtLocation(int i, int j) override { return k_standardCellType; }
protected:
  static constexpr int k_standardCellType = 0;
  DoublePairStore * m_store;
  int m_series;
  SelectableTableView m_selectableTableView;
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
  StoreController * m_storeController;
  bool m_xColumnSelected;
};

}

#endif
