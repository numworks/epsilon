#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "column_title_cell.h"
#include "even_odd_double_buffer_text_cell_with_separator.h"
#include "../shared/hideable_even_odd_cell.h"
#include "../shared/margin_even_odd_message_text_cell.h"
#include "../shared/tab_table_controller.h"
#include "../shared/separator_even_odd_buffer_text_cell.h"
#include "../shared/store_cell.h"

namespace Regression {

class CalculationController : public Shared::TabTableController, public TableViewDataSource, public SelectableTableViewDelegate, public ButtonRowDelegate, public AlternateEmptyViewDelegate  {

public:
  CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store);

  // View Controller
  const char * title() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;

  // TableViewDataSource
  int numberOfRows() override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
private:
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  constexpr static int k_totalNumberOfDoubleBufferRows = 5;
  constexpr static int k_maxNumberOfSingleBufferRows = 11;
  constexpr static int k_numberOfDoubleCalculationCells = Store::k_numberOfSeries * k_totalNumberOfDoubleBufferRows;
  constexpr static int k_numberOfCalculationCells = Store::k_numberOfSeries * k_maxNumberOfSingleBufferRows;
  constexpr static int k_standardCalculationTitleCellType = 0;
  constexpr static int k_r2CellType = 1;
  constexpr static int k_columnTitleCellType = 2;
  constexpr static int k_doubleBufferCalculationCellType = 3;
  constexpr static int k_standardCalculationCellType = 4;
  static constexpr int k_hideableCellType = 5;
  static constexpr int k_regressionCellIndex = 9;

  static constexpr KDCoordinate k_cellHeight = 25;
  static constexpr KDCoordinate k_smallCalculationCellWidth = Ion::Display::Width/2 - Metric::CommonRightMargin/2 - Metric::CommonLeftMargin/2;
  static constexpr KDCoordinate k_cubicCalculationCellWidth = 150;
  static constexpr KDCoordinate k_quarticCalculationCellWidth = 195;
  static constexpr KDCoordinate k_margin = 8;
  static constexpr KDCoordinate k_r2CellMargin = 2;
  static constexpr KDCoordinate k_scrollBarMargin = Metric::CommonRightMargin;
  Responder * tabController() const override;
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  bool hasLinearRegression() const;
  int maxNumberOfCoefficients() const;
  Poincare::Layout m_r2Layout;
  SelectableTableView m_selectableTableView;
  Shared::MarginEvenOddMessageTextCell m_titleCells[k_maxNumberOfDisplayableRows];
  EvenOddExpressionCell m_r2TitleCell;
  ColumnTitleCell m_columnTitleCells[Store::k_numberOfSeries];
  EvenOddDoubleBufferTextCellWithSeparator m_doubleCalculationCells[k_numberOfDoubleCalculationCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell m_hideableCell;
  Store * m_store;
};

}

#endif
