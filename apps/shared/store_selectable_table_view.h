#ifndef APPS_SHARED_STORE_SELECTABLE_TABLE_VIEW_H
#define APPS_SHARED_STORE_SELECTABLE_TABLE_VIEW_H

#include <escher/selectable_table_view.h>
#include "double_pair_store.h"

namespace Shared {

class StoreSelectableTableView : public Escher::SelectableTableView {
// TODO Put this behavior in the SelectableTableView delegate
public:
  StoreSelectableTableView(DoublePairStore * store, Escher::Responder * parentResponder, Escher::TableViewDataSource * dataSource, Escher::SelectableTableViewDataSource * selectionDataSource = nullptr, Escher::SelectableTableViewDelegate * delegate = nullptr);
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool selectNonHiddenCellAtClippedLocation(int i, int j);
  DoublePairStore * m_store;
};

}

#endif
