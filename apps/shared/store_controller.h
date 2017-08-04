#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher.h>
#include "float_pair_store.h"
#include "store_parameter_controller.h"
#include "editable_cell_table_view_controller.h"

namespace Shared {

class StoreController : public EditableCellTableViewController, public ButtonRowDelegate  {
public:
  StoreController(Responder * parentResponder, FloatPairStore * store, ButtonRowController * header);
  const char * title() override;
  int numberOfColumns() override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
protected:
  static constexpr KDCoordinate k_cellWidth = Ion::Display::Width/2 - Metric::CommonRightMargin/2 - Metric::CommonLeftMargin/2;
  constexpr static int k_maxNumberOfEditableCells = 22;
  constexpr static int k_numberOfTitleCells = 2;
  Responder * tabController() const override;
  View * loadView() override;
  void unloadView(View * view) override;
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  int numberOfElements() override;
  int maxNumberOfElements() const override;
  virtual HighlightCell * titleCells(int index) = 0;
  char m_draftTextBuffer[TextField::maxBufferSize()];
  EvenOddEditableTextCell * m_editableCells[k_maxNumberOfEditableCells];
  FloatPairStore * m_store;
  StoreParameterController m_storeParameterController;
};

}

#endif
