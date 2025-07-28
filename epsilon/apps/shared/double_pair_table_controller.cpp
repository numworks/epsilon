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
  m_prefacedTwiceTableView.setMargins(k_margins);
  for (int i = 0; i < k_numberOfHeaderColumns; i++) {
    m_hideableCell[i].setColor(m_selectableTableView.backgroundColor());
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_calculationTitleCells[i].setAlignment(KDGlyph::k_alignRight,
                                            KDGlyph::k_alignCenter);
    m_calculationSymbolCells[i].setAlignment(KDGlyph::k_alignCenter,
                                             KDGlyph::k_alignCenter);
  }
}

void DoublePairTableController::viewWillAppear() {
  m_selectableTableView.resetSizeAndOffsetMemoization();
  TabTableController::viewWillAppear();
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

void DoublePairTableController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() == -1) {
      selectCellAtLocation(0, 1);
    } else {
      selectCellAtLocation(selectedColumn(), selectedRow());
    }
    TabTableController::handleResponderChainEvent(event);
  } else {
    TabTableController::handleResponderChainEvent(event);
  }
}

HighlightCell* DoublePairTableController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  switch (type) {
    case k_hideableCellType:
      return &m_hideableCell[index];
    case k_calculationTitleCellType:
      return &m_calculationTitleCells[index];
    default:
      assert(k_calculationSymbolCellType);
      return &m_calculationSymbolCells[index];
  }
}

int DoublePairTableController::reusableCellCount(int type) const {
  switch (type) {
    case k_hideableCellType:
      return k_numberOfHeaderColumns;
    case k_seriesTitleCellType:
      return k_numberOfSeriesTitleCells;
    case k_calculationTitleCellType:
      return k_maxNumberOfDisplayableRows;
    case k_calculationSymbolCellType:
      return k_maxNumberOfDisplayableRows;
    default:
      assert(type == k_calculationCellType);
      return k_numberOfCalculationCells;
  }
}

int DoublePairTableController::typeAtLocation(int column, int row) const {
  assert(0 <= column && column < numberOfColumns());
  assert(0 <= row && row < numberOfRows());
  if (column <= 1 && row == 0) {
    return k_hideableCellType;
  }
  if (row == 0) {
    return k_seriesTitleCellType;
  }
  if (column == 0) {
    return k_calculationTitleCellType;
  }
  if (column == 1) {
    return k_calculationSymbolCellType;
  }
  return k_calculationCellType;
}

KDCoordinate DoublePairTableController::separatorBeforeColumn(int index) const {
  return typeAtLocation(index, 0) == k_seriesTitleCellType
             ? Metric::TableSeparatorThickness
             : 0;
}

}  // namespace Shared
