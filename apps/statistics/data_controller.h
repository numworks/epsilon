#ifndef STATISTICS_DATA_CONTROLLER_H
#define STATISTICS_DATA_CONTROLLER_H

#include <escher.h>

namespace Statistics {

class DataController : public ViewController, public TableViewDataSource {
public:
  DataController(Responder * parentResponder);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  virtual View * view() override;

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
private:
  static constexpr KDCoordinate k_cellHeight = 30;
  static constexpr KDCoordinate k_cellWidth = 100;
  constexpr static int k_maxNumberOfEditableCells = 20;
  constexpr static int k_numberOfTitleCells = 2;
  Responder * tabController() const;
  //EvenOddEditableTextCell m_editableCells[k_maxNumberOfEditableCells];
  EvenOddPointerTextCell m_titleCells[k_numberOfTitleCells];
  SelectableTableView m_selectableTableView;
};

}

#endif
