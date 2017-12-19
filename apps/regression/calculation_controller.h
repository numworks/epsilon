#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "even_odd_double_buffer_text_cell.h"
#include "../shared/tab_table_controller.h"
#include "../shared/regular_table_view_data_source.h"

namespace Regression {

class CalculationController : public Shared::TabTableController, public Shared::RegularTableViewDataSource, public SelectableTableViewDelegate, public ButtonRowDelegate, public AlternateEmptyViewDelegate  {

public:
  CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store);
  ~CalculationController();
  CalculationController(const CalculationController& other) = delete;
  CalculationController(CalculationController&& other) = delete;
  CalculationController& operator=(const CalculationController& other) = delete;
  CalculationController& operator=(CalculationController&& other) = delete;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
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
  View * loadView() override;
  void unloadView(View * view) override;
  constexpr static int k_totalNumberOfRows = 14;
  constexpr static int k_totalNumberOfColumns = 2;
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  constexpr static int k_totalNumberOfDoubleBufferRows = 5;
  static constexpr KDCoordinate k_cellHeight = 25;
  static constexpr KDCoordinate k_cellWidth = Ion::Display::Width/2 - Metric::CommonRightMargin/2 - Metric::CommonLeftMargin/2;
  EvenOddMessageTextCell * m_titleCells[k_maxNumberOfDisplayableRows];
  EvenOddExpressionCell * m_r2TitleCell;
  Poincare::ExpressionLayout * m_r2Layout;
  EvenOddDoubleBufferTextCell * m_columnTitleCell;
  EvenOddDoubleBufferTextCell * m_doubleCalculationCells[k_totalNumberOfDoubleBufferRows];
  EvenOddBufferTextCell * m_calculationCells[k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows];
  Store * m_store;
};

}

#endif
