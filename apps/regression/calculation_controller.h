#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "even_odd_double_buffer_text_cell.h"
#include "../shared/tab_table_controller.h"

namespace Regression {

class CalculationController : public Shared::TabTableController, public ButtonRowDelegate, public AlternateEmptyViewDelegate, public SelectableTableViewDelegate {

public:
  CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store);
  ~CalculationController();
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  bool isEmpty() const override;
  const char * emptyMessage() override;
  Responder * defaultController() override;

  int numberOfRows() override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
private:
  Responder * tabController() const override;
  constexpr static int k_totalNumberOfRows = 11;
  constexpr static int k_totalNumberOfColumns = 2;
  constexpr static int k_maxNumberOfDisplayableRows = 10;
  static constexpr KDCoordinate k_cellHeight = 25;
  static constexpr KDCoordinate k_cellWidth = Ion::Display::Width/2 - Metric::CommonRightMargin/2 - Metric::CommonLeftMargin/2;
  EvenOddPointerTextCell m_titleCells[k_maxNumberOfDisplayableRows];
  EvenOddExpressionCell m_r2TitleCell;
  Poincare::ExpressionLayout * m_r2Layout;
  EvenOddDoubleBufferTextCell m_columnTitleCell;
  EvenOddDoubleBufferTextCell m_doubleCalculationCells[k_maxNumberOfDisplayableRows/2];
  EvenOddBufferTextCell m_calculationCells[k_maxNumberOfDisplayableRows/2];
  Store * m_store;
};

}

#endif
