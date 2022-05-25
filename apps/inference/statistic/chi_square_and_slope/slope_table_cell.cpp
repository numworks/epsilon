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
  // TODO: use Felix constexpr
  m_header[0].setText("X1");
  m_header[1].setText("Y1");
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_header[i].setColor(Escher::Palette::Red);
    m_header[i].setOrientation(Shared::FunctionTitleCell::Orientation::HorizontalIndicator);
    m_header[i].setEven(true);
    m_header[i].setFont(KDFont::SmallFont);
  }
}

void SlopeTableCell::didEnterResponderChain(Responder * previousResponder) {
  m_selectableTableView.reloadData(false, false);
}

InputViewController * SlopeTableCell::inputViewController() {
  return App::app()->inputViewController();
}

}  // namespace Inference
