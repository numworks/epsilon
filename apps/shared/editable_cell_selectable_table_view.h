#ifndef APPS_SHARED_STORE_SELECTABLE_TABLE_VIEW_H
#define APPS_SHARED_STORE_SELECTABLE_TABLE_VIEW_H

#include <escher/selectable_table_view.h>

namespace Shared {

class EditableCellTableViewController;

class EditableCellSelectableTableView : public Escher::SelectableTableView {
 public:
  EditableCellSelectableTableView(
      EditableCellTableViewController* tableViewController,
      Escher::Responder* parentResponder,
      Escher::TableViewDataSource* dataSource,
      Escher::SelectableTableViewDataSource* selectionDataSource = nullptr,
      Escher::SelectableTableViewDelegate* delegate = nullptr);
  bool handleEvent(Ion::Events::Event event) override;

 private:
  bool selectNonHiddenCellAtClippedLocation(int i, int j);
  EditableCellTableViewController* m_tableViewController;
};

}  // namespace Shared

#endif
