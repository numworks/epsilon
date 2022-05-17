#include "slope_table_cell.h"
#include <escher/palette.h>
#include "inference/statistic/chi_square_and_slope/input_goodness_controller.h"

using namespace Escher;

namespace Inference {

SlopeTableCell::SlopeTableCell(Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, SelectableTableViewDelegate * selectableTableViewDelegate, Table * tableModel) :
  DoubleColumnTableCell(parentResponder, dynamicSizeTableViewDataSourceDelegate, selectableTableViewDelegate, tableModel)
{
  // TODO: use Felix constexpr
  m_header[0].setText("X1");
  m_header[0].setText("Y1");
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_header[i].setColor(Escher::Palette::Red);
    m_header[i].setEven(true);
    m_header[i].setFont(KDFont::SmallFont);
  }
}

}  // namespace Inference
