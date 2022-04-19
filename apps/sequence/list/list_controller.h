#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <escher/table_view_data_source.h>
#include "../../shared/sequence_title_cell.h"
#include "../../shared/sequence_store.h"
#include "../../shared/function_expression_cell.h"
#include "../../shared/function_list_controller.h"
#include "../../shared/input_event_handler_delegate.h"
#include "../../shared/layout_field_delegate.h"
#include "../../shared/text_field_delegate.h"
#include "list_parameter_controller.h"
#include "sequence_toolbox.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController : public Shared::FunctionListController, public Shared::InputEventHandlerDelegate, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate, public Escher::TableViewDataSource {
public:
  ListController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, Escher::ButtonRowController * footer);
  const char * title() override;
  int numberOfExpressionRows() const override;
  KDCoordinate expressionRowHeight(int j) override;
  Escher::Toolbox * toolboxForInputEventHandler(Escher::InputEventHandler * handler) override;
  void selectPreviousNewSequenceCell();
  void editExpression(int sequenceDefinitionIndex, Ion::Events::Event event);
  /* ViewController */
  void viewWillAppear() override;
  /* TableViewDataSource */
  int numberOfRows() const override { return this->numberOfExpressionRows(); }
  KDCoordinate rowHeight(int j) override { return ExpressionModelListController::memoizedRowHeight(j); }
  KDCoordinate cumulatedHeightFromIndex(int j) override { return ExpressionModelListController::memoizedCumulatedHeightFromIndex(j); }
  int indexFromCumulatedHeight(KDCoordinate offsetY) override { return ExpressionModelListController::memoizedIndexFromCumulatedHeight(offsetY); }
  int numberOfColumns() const override { return 2; }
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int typeAtLocation(int i, int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  /* Responder */
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  /* ViewController */
  Escher::View * view() override { return &m_selectableTableView; }
  /* SelectableTableViewDelegate*/
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
  /* ExpressionModelListController */
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
private:
  static constexpr KDCoordinate k_minTitleColumnWidth = 65;
  static constexpr KDCoordinate k_functionTitleSumOfMargins = 25;
  static constexpr KDCoordinate k_expressionCellVerticalMargin = 3;
  static constexpr int k_maxNumberOfRows = 3*Shared::SequenceStore::k_maxNumberOfSequences;
  void computeTitlesColumnWidth(bool forceMax = false);
  void resetMemoizationForIndex(int index) override;
  void shiftMemoization(bool newCellIsUnder) override;
  bool editInitialConditionOfSelectedRecordWithText(const char * text, bool firstInitialCondition);
  ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Shared::FunctionTitleCell * titleCells(int index);
  Escher::HighlightCell * functionCells(int index) override;
  void willDisplayTitleCellAtIndex(Escher::HighlightCell * cell, int j);
  void willDisplayExpressionCellAtIndex(Escher::HighlightCell * cell, int j) override;
  int modelIndexForRow(int j) override;
  int sequenceDefinitionForRow(int j);
  KDCoordinate maxFunctionNameWidth();
  void didChangeModelsList() override;
  KDCoordinate baseline(int j);
  KDCoordinate privateBaseline(int j) const;
  KDCoordinate notMemoizedCumulatedHeightFromIndex(int j) override {
    return TableViewDataSource::cumulatedHeightFromIndex(j);
  }
  int notMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) override {
    return TableViewDataSource::indexFromCumulatedHeight(offsetY);
  }
  void addModel() override;
  void editExpression(Ion::Events::Event event) override;
  bool removeModelRow(Ion::Storage::Record record) override;
  Shared::SequenceStore * modelStore() override;
  KDCoordinate nameWidth(int nameLength) const;
  Escher::SelectableTableView m_selectableTableView;
  Escher::EvenOddCell m_emptyCell;
  Shared::SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  Escher::StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
  KDCoordinate m_titlesColumnWidth;
  KDCoordinate m_memoizedCellBaseline[k_memoizedCellsCount];
};

}

#endif
