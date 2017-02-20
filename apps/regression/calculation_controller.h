#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "even_odd_double_buffer_text_cell.h"

namespace Regression {

class CalculationController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate, public TableViewDataSource, public SelectableTableViewDelegate {

public:
  CalculationController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  bool isEmpty() const override;
  const char * emptyMessage() override;
  Responder * defaultController() override;

  int numberOfRows() override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void viewWillAppear() override;
private:
  Responder * tabController() const;
  constexpr static int k_totalNumberOfRows = 11;
  constexpr static int k_totalNumberOfColumns = 2;
  constexpr static int k_maxNumberOfDisplayableRows = 10;
  static constexpr KDCoordinate k_cellHeight = 25;
  static constexpr KDCoordinate k_cellWidth = Ion::Display::Width/2 - Metric::RightMargin/2 - Metric::LeftMargin/2;
  EvenOddPointerTextCell m_titleCells[k_maxNumberOfDisplayableRows];
  EvenOddDoubleBufferTextCell m_columnTitleCell;
  EvenOddDoubleBufferTextCell m_doubleCalculationCells[k_maxNumberOfDisplayableRows/2];
  EvenOddBufferTextCell m_calculationCells[k_maxNumberOfDisplayableRows/2];
  SelectableTableView m_selectableTableView;
  Store * m_store;
};

}

#endif
