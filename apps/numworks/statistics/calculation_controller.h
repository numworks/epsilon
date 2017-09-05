#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "apps/shared/tab_table_controller.h"
#include "apps/shared/regular_table_view_data_source.h"


namespace Statistics {

class CalculationController : public Shared::TabTableController, public  Shared::RegularTableViewDataSource, public ButtonRowDelegate, public AlternateEmptyViewDelegate {

public:
  CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

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
  constexpr static int k_totalNumberOfRows = 13;
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_cellWidth = Ion::Display::Width/2 - Metric::CommonRightMargin/2 - Metric::CommonLeftMargin/2;
  EvenOddMessageTextCell * m_titleCells[k_maxNumberOfDisplayableRows];
  EvenOddBufferTextCell * m_calculationCells[k_maxNumberOfDisplayableRows];
  Store * m_store;
};

}


#endif
