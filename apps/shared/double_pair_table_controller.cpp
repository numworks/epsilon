#include "double_pair_table_controller.h"

using namespace Escher;

namespace Shared {

DoublePairTableController::DoublePairTableController(Responder * parentResponder, ButtonRowController * header) :
  TabTableController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_prefacedView(0, this, &m_selectableTableView, this, this),
  m_selectableTableView(this, this, this, &m_prefacedView)
{
  m_prefacedView.setCellOverlap(0, 0);
  m_prefacedView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
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

}
