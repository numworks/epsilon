#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher.h>
#include "buffer_text_view_with_text_field.h"
#include "editable_cell_table_view_controller.h"
#include "float_pair_store.h"
#include "store_cell.h"
#include "store_parameter_controller.h"
#include "store_selectable_table_view.h"

namespace Shared {

class StoreController : public EditableCellTableViewController, public ButtonRowDelegate  {
public:
  StoreController(Responder * parentResponder, FloatPairStore * store, ButtonRowController * header);

  void displayFormulaInput();
  virtual void setFormulaLabel() = 0;
  virtual void fillColumnWithFormula(Poincare::Expression * formula) = 0;

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
  static constexpr KDCoordinate k_cellWidth = 116;
  static constexpr KDCoordinate k_margin = 8;
  static constexpr KDCoordinate k_scrollBarMargin = Metric::CommonRightMargin;
  constexpr static int k_maxNumberOfEditableCells = 22 * FloatPairStore::k_numberOfSeries;
  constexpr static int k_numberOfTitleCells = 4;
  static constexpr int k_titleCellType = 0;
  static constexpr int k_editableCellType = 1;

  class ContentView : public View , public Responder {
  public:
    ContentView(FloatPairStore * store, Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource, TextFieldDelegate * textFieldDelegate);
   StoreSelectableTableView * dataView() { return &m_dataView; }
   BufferTextViewWithTextField * formulaInputView() { return &m_formulaInputView; }
   void displayFormulaInput(bool display);
  // Responder
  void didBecomeFirstResponder() override;
  private:
    static constexpr KDCoordinate k_margin = 8;
    static constexpr KDCoordinate k_scrollBarMargin = Metric::CommonRightMargin;
    int numberOfSubviews() const override { return 1 + m_displayFormulaInputView; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    KDRect formulaFrame() const;
    StoreSelectableTableView m_dataView;
    BufferTextViewWithTextField m_formulaInputView;
    bool m_displayFormulaInputView;
  };

  Responder * tabController() const override;
  SelectableTableView * selectableTableView() override;
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
  StoreCell * m_editableCells[k_maxNumberOfEditableCells];
  FloatPairStore * m_store;
  StoreParameterController m_storeParameterController;
private:
  bool cellShouldBeTransparent(int i, int j);
  ContentView * contentView() { return static_cast<ContentView *>(view()); }
};

}

#endif
