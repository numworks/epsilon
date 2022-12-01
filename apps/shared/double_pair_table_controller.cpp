#include "double_pair_table_controller.h"

using namespace Escher;

namespace Shared {

DoublePairTableController::DoublePairTableController(Responder * parentResponder, ButtonRowController * header) :
  TabTableController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_prefacedTwiceTableView(0, 1, this, &m_selectableTableView, this, this),
  m_selectableTableView(this, this, this, &m_prefacedTwiceTableView)
{
  m_prefacedTwiceTableView.setCellOverlap(0, 0);
  m_prefacedTwiceTableView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedTwiceTableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  m_prefacedTwiceTableView.setMarginDelegate(this);
}

bool DoublePairTableController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    Escher::Container::activeApp()->setFirstResponder(tabController());
    return true;
  }
  return false;
}

void DoublePairTableController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 1);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  TabTableController::didBecomeFirstResponder();
}

KDCoordinate DoublePairTableController::prefaceMargin(TableView * preface, TableViewDataSource * prefaceDataSource) {
  if (prefaceDataSource->numberOfColumns() > 1) {
    return 0;
  }
  KDCoordinate prefaceRightSide = offset().x() + (preface->bounds().isEmpty() ? preface->minimalSizeForOptimalDisplay().width() : 0);

  for (int i = 0; i < numberOfColumns(); i++) {
    constexpr KDCoordinate maxMargin = Escher::Metric::TableSeparatorThickness;
    KDCoordinate delta = prefaceRightSide - cumulatedWidthBeforeIndex(i);
    if (delta < 0) {
      return maxMargin;
    } else if (delta <= maxMargin) {
      return delta;
    }
  }
  assert(false);
  return 0;
}

}
