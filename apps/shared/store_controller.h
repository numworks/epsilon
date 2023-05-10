#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher.h>
#include "buffer_text_view_with_text_field.h"
#include "editable_cell_table_view_controller.h"
#include "double_pair_store.h"
#include "store_cell.h"
#include "store_context.h"
#include "store_parameter_controller.h"
#include "store_selectable_table_view.h"

namespace Shared {

class StoreController : public EditableCellTableViewController, public ButtonRowDelegate  {
public:
  StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, DoublePairStore * store, ButtonRowController * header);
  View * view() override { return &m_contentView; }
  TELEMETRY_ID("Store");

  virtual StoreContext * storeContext() = 0;
  void displayFormulaInput();
  virtual void setFormulaLabel() = 0;
  virtual bool fillColumnWithFormula(Poincare::Expression formula) = 0;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;

  // TableViewDataSource
  int numberOfColumns() const override;
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
  static constexpr KDCoordinate k_cellWidth = Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::LargeNumberOfSignificantDigits) * 7 + 2*Metric::CellMargin + Metric::TableSeparatorThickness; // KDFont::SmallFont->glyphSize().width() = 7

  constexpr static int k_maxNumberOfEditableCells = (Ion::Display::Width/k_cellWidth+2) * ((Ion::Display::Height - Metric::TitleBarHeight - Metric::TabHeight)/k_cellHeight+2);
  #ifndef _FXCG
  constexpr static int k_numberOfTitleCells = 4;
  #else
  // This is different here due to the changed screen resolution
  constexpr static int k_numberOfTitleCells = 5;
  #endif
  static constexpr int k_titleCellType = 0;
  static constexpr int k_editableCellType = 1;

  class ContentView : public View , public Responder {
  public:
    ContentView(DoublePairStore * store, Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate);
   StoreSelectableTableView * dataView() { return &m_dataView; }
   BufferTextViewWithTextField * formulaInputView() { return &m_formulaInputView; }
   void displayFormulaInput(bool display);
  // Responder
  void didBecomeFirstResponder() override;
  private:
    static constexpr KDCoordinate k_formulaInputHeight = 31;
    int numberOfSubviews() const override { return 1 + m_displayFormulaInputView; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    KDRect formulaFrame() const;
    StoreSelectableTableView m_dataView;
    BufferTextViewWithTextField m_formulaInputView;
    bool m_displayFormulaInputView;
  };

  Responder * tabController() const override;
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  virtual HighlightCell * titleCells(int index) = 0;
  int seriesAtColumn(int column) const { return column / DoublePairStore::k_numberOfColumnsPerSeries; }
  bool privateFillColumnWithFormula(Poincare::Expression formula, Poincare::ExpressionNode::isVariableTest isVariable);
  virtual StoreParameterController * storeParameterController() = 0;
  StoreCell m_editableCells[k_maxNumberOfEditableCells];
  DoublePairStore * m_store;
private:
  SelectableTableView * selectableTableView() override {
    return m_contentView.dataView();
  }
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  int numberOfElementsInColumn(int columnIndex) const override;
  int maxNumberOfElements() const override {
    return DoublePairStore::k_maxNumberOfPairs;
  };
  ContentView m_contentView;
};

}

#endif
