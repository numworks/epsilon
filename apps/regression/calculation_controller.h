#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <poincare/preferences.h>
#include "store.h"
#include "column_title_cell.h"
#include "even_odd_double_buffer_text_cell_with_separator.h"
#include "../shared/hideable_even_odd_cell.h"
#include "../shared/tab_table_controller.h"
#include "../shared/separator_even_odd_buffer_text_cell.h"
#include "../shared/store_cell.h"

namespace Regression {

constexpr static KDCoordinate maxCoordinate(KDCoordinate a, KDCoordinate b) { return a > b ? a : b; }

class CalculationController : public Shared::TabTableController, public Escher::TableViewDataSource, public Escher::SelectableTableViewDelegate, public Escher::ButtonRowDelegate, public Escher::AlternateEmptyViewDefaultDelegate  {

public:
  CalculationController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Store * store);

  // View Controller
  const char * title() override;
  TELEMETRY_ID("Calculation");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // AlternateEmptyViewDefaultDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Escher::Responder * defaultController() override;

  // TableViewDataSource
  int numberOfRows() const override;
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
private:
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  constexpr static int k_totalNumberOfDoubleBufferRows = 5;
  constexpr static int k_numberOfDoubleCalculationCells = Store::k_numberOfSeries * k_totalNumberOfDoubleBufferRows;
  constexpr static int k_numberOfCalculationCells = Store::k_numberOfSeries * k_maxNumberOfDisplayableRows;
  constexpr static int k_standardCalculationTitleCellType = 0;
  constexpr static int k_columnTitleCellType = 1;
  constexpr static int k_doubleBufferCalculationCellType = 2;
  constexpr static int k_standardCalculationCellType = 3;
  static constexpr int k_hideableCellType = 4;
  static constexpr int k_regressionCellIndex = 9;

  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_titleCalculationCellWidth = Ion::Display::Width/2 - Escher::Metric::CommonRightMargin/2 - Escher::Metric::CommonLeftMargin/2;
  /* Separator and margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here.
   * TODO: change 7 for KDFont::SmallFont->glyphSize().width()
   * Calculation width should at least be able to hold to numbers with
   * VeryLargeNumberOfSignificantDigits and contains two even odd cells. */
  static constexpr KDCoordinate k_minCalculationCellWidth = 2*(7*Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits)+2*Escher::EvenOddCell::k_horizontalMargin)+Escher::EvenOddCell::k_separatorWidth;
  // To hold _y=a·x^3+b·x^2+c·x+d_
  static constexpr KDCoordinate k_cubicCalculationCellWidth = maxCoordinate(7*21+2*Escher::EvenOddCell::k_horizontalMargin+Escher::EvenOddCell::k_separatorWidth, k_minCalculationCellWidth);
  // To hold _y=a·x^4+b·x^3+c·x^2+d·x+e_
  static constexpr KDCoordinate k_quarticCalculationCellWidth = maxCoordinate(7*27+2*Escher::EvenOddCell::k_horizontalMargin+Escher::EvenOddCell::k_separatorWidth, k_minCalculationCellWidth);
  static constexpr KDCoordinate k_margin = 8;
  static constexpr KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;
  Escher::Responder * tabController() const override;
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  bool hasLinearRegression() const;
  int maxNumberOfCoefficients() const;
  Escher::SelectableTableView m_selectableTableView;
  Escher::EvenOddMessageTextCell m_titleCells[k_maxNumberOfDisplayableRows];
  ColumnTitleCell m_columnTitleCells[Store::k_numberOfSeries];
  EvenOddDoubleBufferTextCellWithSeparator m_doubleCalculationCells[k_numberOfDoubleCalculationCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell m_hideableCell;
  Store * m_store;
};

}

#endif
