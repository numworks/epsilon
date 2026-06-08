#include <escher/table_grid_view.h>
#include <escher/table_view.h>
#include <escher/theme/color_picker.h>

namespace Escher {

TableGridView::TableGridView(const TableView* tableView)
    : m_tableView(tableView) {}

KDSize TableGridView::minimalSizeForOptimalDisplay() const {
  return KDSize(table()->bounds().width(), table()->bounds().height());
}

}  // namespace Escher
