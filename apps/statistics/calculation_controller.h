#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
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
  int numberOfColumns() override { return 2; }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override { return k_cellHeight; }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_maxNumberOfDisplayableRows; }
  int typeAtLocation(int i, int j) override;

  // ViewController
  const char * title() override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::DoNotShowOwnTitle; }

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  Responder * tabController() const override;
  View * loadView() override;
  void unloadView(View * view) override;
  constexpr static int k_totalNumberOfRows = 14;
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_titleCellWidth = 175;
  EvenOddMessageTextCell * m_titleCells[k_maxNumberOfDisplayableRows];
  EvenOddBufferTextCell * m_calculationCells[k_maxNumberOfDisplayableRows];
  Store * m_store;
};

}


#endif
