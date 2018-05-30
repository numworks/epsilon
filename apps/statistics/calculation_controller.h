#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "calculation_cell.h"
#include "calculation_title_cell.h"
#include "../shared/hideable_even_odd_cell.h"
#include "../shared/store_title_cell.h"
#include "../shared/tab_table_controller.h"

namespace Statistics {

class CalculationController : public Shared::TabTableController, public ButtonRowDelegate, public TableViewDataSource, public AlternateEmptyViewDelegate {

public:
  CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;

  // TableViewDataSource
  int numberOfRows() override { return k_totalNumberOfRows; }
  int numberOfColumns() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override { return k_cellHeight; }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

  // ViewController
  const char * title() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  static constexpr int k_totalNumberOfRows = 15;
  static constexpr int k_maxNumberOfDisplayableRows = 11;
  static constexpr int k_numberOfCalculationCells = 3 * k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfSeriesTitleCells = 3;
  static constexpr int k_numberOfCalculationTitleCells = k_maxNumberOfDisplayableRows;

  static constexpr int k_calculationTitleCellType = 0;
  static constexpr int k_calculationCellType = 1;
  static constexpr int k_seriesTitleCellType = 2;
  static constexpr int k_hideableCellType = 3;
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_calculationTitleCellWidth = 175;
  static constexpr KDCoordinate k_calculationCellWidth = 84;
  static constexpr KDCoordinate k_margin = 8;
  static constexpr KDCoordinate k_scrollBarMargin = Metric::CommonRightMargin;

  Responder * tabController() const override;
  View * loadView() override;
  void unloadView(View * view) override;

  Shared::StoreTitleCell * m_seriesTitleCells[k_numberOfSeriesTitleCells];
  CalculationTitleCell * m_calculationTitleCells[k_numberOfCalculationTitleCells];
  CalculationCell * m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell * m_hideableCell;
  Store * m_store;
};

}


#endif
