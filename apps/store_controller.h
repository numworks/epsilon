#ifndef APPS_STORE_CONTROLLER_H
#define APPS_STORE_CONTROLLER_H

#include <escher.h>
#include "float_pair_store.h"
#include "store_parameter_controller.h"
#include "editable_cell_table_view_controller.h"

class StoreController : public EditableCellTableViewController, public HeaderViewDelegate  {
public:
  StoreController(Responder * parentResponder, FloatPairStore * store, HeaderViewController * header);
  const char * title() const override;
  int numberOfColumns() override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  static constexpr KDCoordinate k_cellWidth = Ion::Display::Width/2 - Metric::RightMargin/2 - Metric::LeftMargin/2;
  constexpr static int k_maxNumberOfEditableCells = 22;
  constexpr static int k_numberOfTitleCells = 2;
  Responder * tabController() const;
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  void setDataAtLocation(float floatBody, int columnIndex, int rowIndex) override;
  float dataAtLocation(int columnIndex, int rowIndex) override;
  int numberOfElements() override;
  int maxNumberOfElements() const override;
  virtual TableViewCell * titleCells(int index) = 0;
  char m_draftTextBuffer[EditableTextCell::k_bufferLength];
  EvenOddEditableTextCell m_editableCells[k_maxNumberOfEditableCells];
  FloatPairStore * m_store;
  StoreParameterController m_storeParameterController;
};

#endif
