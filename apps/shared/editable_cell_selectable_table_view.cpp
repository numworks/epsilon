#include "editable_cell_selectable_table_view.h"

#include "editable_cell_table_view_controller.h"

using namespace Escher;

namespace Shared {

EditableCellSelectableTableView::EditableCellSelectableTableView(
    EditableCellTableViewController* tableViewController,
    Responder* parentResponder, TableViewDataSource* dataSource,
    SelectableTableViewDataSource* selectionDataSource,
    SelectableTableViewDelegate* delegate)
    : SelectableTableView(parentResponder, dataSource, selectionDataSource,
                          delegate),
      m_tableViewController(tableViewController) {}

bool EditableCellSelectableTableView::handleEvent(Ion::Events::Event event) {
  int step = Ion::Events::longPressFactor();
  if (event == Ion::Events::Down) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn(),
                                                selectedRow() + step);
  }
  if (event == Ion::Events::Up) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn(),
                                                selectedRow() - step);
  }
  if (event == Ion::Events::Left) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn() - step,
                                                selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectNonHiddenCellAtClippedLocation(selectedColumn() + step,
                                                selectedRow());
  }
  return SelectableTableView::handleEvent(event);
}

bool EditableCellSelectableTableView::selectNonHiddenCellAtClippedLocation(
    int i, int j) {
  i = std::clamp<int>(i, 0, totalNumberOfColumns() - 1);
  j = std::clamp<int>(j, 0, m_tableViewController->numberOfRowsAtColumn(i) - 1);
  return selectCellAtClippedLocation(i, j);
}

}  // namespace Shared
