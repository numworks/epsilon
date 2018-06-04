#ifndef APPS_SHARED_STORE_SELECTABLE_TABLE_VIEW_H
#define APPS_SHARED_STORE_SELECTABLE_TABLE_VIEW_H

#include <escher/selectable_table_view.h>
#include "double_pair_store.h"

namespace Shared {

class StoreSelectableTableView : public SelectableTableView {
public:
  StoreSelectableTableView(DoublePairStore * store, Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource = nullptr, SelectableTableViewDelegate * delegate = nullptr);
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool selecNonHiddenCellAtLocation(int i, int j);
  DoublePairStore * m_store;
};

}

#endif
