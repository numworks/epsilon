#include "double_pair_table_controller.h"

using namespace Escher;

namespace Shared {

DoublePairTableController::DoublePairTableController(
    Responder* parentResponder, ButtonRowController* header)
    : TabTableController(parentResponder),
      ButtonRowDelegate(header, nullptr),
      m_prefacedTwiceTableView(0, 1, this, &m_selectableTableView, this, this),
      m_selectableTableView(this, this, this, &m_prefacedTwiceTableView) {
  m_prefacedTwiceTableView.setCellOverlap(0, 0);
  m_prefacedTwiceTableView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedTwiceTableView.setMargins(k_margin, k_scrollBarMargin,
                                      k_scrollBarMargin, k_margin);
  m_prefacedTwiceTableView.setMarginDelegate(this);
  for (int i = 0; i < k_numberOfHeaderColumns; i++) {
    m_hideableCell[i].setColor(m_selectableTableView.backgroundColor());
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_calculationTitleCells[i].setAlignment(KDGlyph::k_alignRight,
                                            KDGlyph::k_alignCenter);
    m_calculationTitleCells[i].setMessageFont(KDFont::Size::Small);
  }
}

Responder* DoublePairTableController::responderWhenEmpty() {
  tabController()->selectTab();
  return tabController();
}

bool DoublePairTableController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    tabController()->selectTab();
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

KDCoordinate DoublePairTableController::columnPrefaceRightMargin() {
  KDCoordinate actualOffset =
      offset().x() +
      (m_prefacedTwiceTableView.columnPrefaceView()->bounds().isEmpty()
           ? m_prefacedTwiceTableView.columnPrefaceView()
                 ->minimalSizeForOptimalDisplay()
                 .width()
           : 0);

  for (int i = 0; i < numberOfColumns(); i++) {
    constexpr KDCoordinate maxMargin = Metric::TableSeparatorThickness;
    KDCoordinate delta = actualOffset - cumulatedWidthBeforeColumn(i);
    if (delta < 0) {
      return maxMargin;
    } else if (delta <= maxMargin) {
      return delta;
    }
  }
  assert(false);
  return 0;
}

}  // namespace Shared
