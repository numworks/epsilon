#include "slope_table_cell.h"
#include <escher/palette.h>
#include "inference/app.h"
#include "inference/statistic/chi_square_and_slope/input_goodness_controller.h"

using namespace Escher;

namespace Inference {

SlopeTableCell::SlopeTableCell(Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, SelectableTableViewDelegate * selectableTableViewDelegate, Statistic * statistic, Poincare::Context * parentContext) :
  DoubleColumnTableCell(parentResponder, dynamicSizeTableViewDataSourceDelegate, selectableTableViewDelegate, statistic),
  StoreColumnHelper(this, parentContext, this)
{
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_header[i].setColor(Escher::Palette::Red);
    m_header[i].setOrientation(Shared::FunctionTitleCell::Orientation::HorizontalIndicator);
    m_header[i].setEven(true);
    m_header[i].setFont(KDFont::SmallFont);
  }
}

void SlopeTableCell::didEnterResponderChain(Responder * previousResponder) {
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    /* We delayed filling the column names X1, Y1 to ensure that the underlying
     * model was a valid DoublePairStore. */
    fillColumnName(i, const_cast<char *>(m_header[i].text()));
  }
  m_selectableTableView.reloadData(false, false);
}

InputViewController * SlopeTableCell::inputViewController() {
  return App::app()->inputViewController();
}

}  // namespace Inference
