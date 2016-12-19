#ifndef STATISTICS_DATA_CONTROLLER_H
#define STATISTICS_DATA_CONTROLLER_H

#include <escher.h>
#include "data.h"

namespace Statistics {

class DataController : public ViewController, public TableViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate {
public:
  DataController(Responder * parentResponder, Data * m_data);
  const char * title() const override;
  virtual View * view() override;

  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

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

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  static constexpr KDCoordinate k_cellHeight = 30;
  static constexpr KDCoordinate k_cellWidth = 100;
  constexpr static int k_maxNumberOfEditableCells = 20;
  constexpr static int k_numberOfTitleCells = 2;
  Responder * tabController() const;
  char m_draftTextBuffer[EditableTextCell::k_bufferLength];
  EvenOddEditableTextCell m_editableCells[k_maxNumberOfEditableCells];
  EvenOddPointerTextCell m_titleCells[k_numberOfTitleCells];
  SelectableTableView m_selectableTableView;
  Data * m_data;
};

}

#endif
