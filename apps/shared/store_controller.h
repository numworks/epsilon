#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher.h>
#include "editable_cell_table_view_controller.h"
#include "float_pair_store.h"
#include "hideable_even_odd_editable_text_cell.h"
#include "store_parameter_controller.h"

namespace Shared {

class StoreController : public EditableCellTableViewController, public ButtonRowDelegate  {
public:
  StoreController(Responder * parentResponder, FloatPairStore * store, ButtonRowController * header);

  // TextFieldDelegate
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;

  // TableViewDataSource
  int numberOfColumns() override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  // ViewController
  const char * title() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

protected:
  static constexpr KDCoordinate k_cellWidth = 80; //TODO
  static constexpr KDCoordinate k_margin = 8;
  constexpr static int k_maxNumberOfEditableCells = 22 * FloatPairStore::k_numberOfSeries;
  constexpr static int k_numberOfTitleCells = FloatPairStore::k_numberOfColumnsPerSeries * FloatPairStore::k_numberOfSeries;  // TODO Put finer number of cells
  static constexpr int k_titleCellType = 0;
  static constexpr int k_editableCellType = 1;
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
  int seriesAtColumn(int column) const { return column / FloatPairStore::k_numberOfColumnsPerSeries; }
  HideableEvenOddEditableTextCell * m_editableCells[k_maxNumberOfEditableCells];
  FloatPairStore * m_store;
  StoreParameterController m_storeParameterController;
private:
  bool cellShouldBeTransparent(int i, int j);
};

}

#endif
