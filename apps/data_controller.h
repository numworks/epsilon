#ifndef APPS_DATA_CONTROLLER_H
#define APPS_DATA_CONTROLLER_H

#include <escher.h>
#include "data.h"
#include "editable_cell_table_view_controller.h"

class DataController : public EditableCellTableViewController {
public:
  DataController(Responder * parentResponder, Data * m_data);
  const char * title() const override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  static constexpr KDCoordinate k_cellWidth = 100;
  constexpr static int k_maxNumberOfEditableCells = 20;
  constexpr static int k_numberOfTitleCells = 2;
  Responder * tabController() const;
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  void setDataAtLocation(float floatBody, int columnIndex, int rowIndex) override;
  float dataAtLocation(int columnIndex, int rowIndex) override;
  int numberOfElements() override;
  int maxNumberOfElements() const override;
  char m_draftTextBuffer[EditableTextCell::k_bufferLength];
  EvenOddEditableTextCell m_editableCells[k_maxNumberOfEditableCells];
  EvenOddPointerTextCell m_titleCells[k_numberOfTitleCells];
  Data * m_data;
};

#endif
