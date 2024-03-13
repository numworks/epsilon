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
    return selectCellAtClippedLocation(selectedColumn(), selectedRow() + step);
  }
  if (event == Ion::Events::Up) {
    return selectCellAtClippedLocation(selectedColumn(), selectedRow() - step);
  }
  if (event == Ion::Events::Left) {
    return selectCellAtClippedLocation(selectedColumn() - step, selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectCellAtClippedLocation(selectedColumn() + step, selectedRow());
  }
  return SelectableTableView::handleEvent(event);
}

}  // namespace Shared
