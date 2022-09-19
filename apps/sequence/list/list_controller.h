#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/sequence_store.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/input_event_handler_delegate.h>
#include <apps/shared/layout_field_delegate.h>
#include <apps/shared/text_field_delegate.h>
#include <escher/table_view_data_source.h>
#include "list_parameter_controller.h"
#include "sequence_toolbox.h"
#include "type_parameter_controller.h"
#include "vertical_sequence_title_cell.h"

namespace Sequence {

class ListController : public Shared::FunctionListController, public Shared::InputEventHandlerDelegate, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate, public Escher::TableViewDataSource {
public:
  ListController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, Escher::ButtonRowController * footer);
  const char * title() override { return I18n::translate(I18n::Message::SequenceTab); }
  int numberOfExpressionRows() const override;
  KDCoordinate expressionRowHeight(int j) override;
  Escher::Toolbox * toolboxForInputEventHandler(Escher::InputEventHandler * handler) override;
  void selectPreviousNewSequenceCell();
  void editExpression(int sequenceDefinitionIndex, Ion::Events::Event event);
  /* ViewController */
  void viewWillAppear() override;
  /* TableViewDataSource */
  int numberOfRows() const override { return this->numberOfExpressionRows(); }
  int numberOfColumns() const override { return 2; }
  int typeAtLocation(int i, int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return type > k_expressionCellType ? 1 : maxNumberOfDisplayableRows(); }
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
  void showLastSequence();
  /* LayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
private:
  /* Cell types */
  constexpr static int k_titleCellType = 0;
  constexpr static int k_expressionCellType = k_titleCellType + 1;
  constexpr static int k_emptyRowCellType = k_expressionCellType + 1;
  constexpr static int k_addModelCellType = k_emptyRowCellType + 1;
  /* Model definitions */
  constexpr static int k_otherDefinition = -1;
  constexpr static int k_sequenceDefinition = 0;
  constexpr static int k_firstInitialCondition = k_sequenceDefinition + 1;
  constexpr static int k_secondInitialCondition = k_firstInitialCondition + 1;
  /* Width and margins */
  constexpr static KDCoordinate k_minTitleColumnWidth = 65;
  constexpr static KDCoordinate k_functionTitleSumOfMargins = 25;
  constexpr static KDCoordinate k_expressionCellVerticalMargin = Escher::Metric::BigCellMargin;
  /* Row numbers */
  constexpr static int k_maxNumberOfRows = 3*Shared::SequenceStore::k_maxNumberOfSequences;

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int i) override;
  KDCoordinate nonMemoizedRowHeight(int j) override { return expressionRowHeight(j); }
  Escher::TableSize1DManager * rowHeightManager() override { return &m_heightManager; }

  // ExpressionModelListController
  void resetSizesMemoization() override { resetMemoization(); }

  void computeTitlesColumnWidth(bool forceMax = false);
  bool editInitialConditionOfSelectedRecordWithText(const char * text, bool firstInitialCondition);
  ListParameterController * parameterController() override { return &m_parameterController; }
  int maxNumberOfDisplayableRows() override { return k_maxNumberOfRows; }
  Escher::HighlightCell * titleCells(int index);
  Escher::HighlightCell * functionCells(int index) override;
  void willDisplayTitleCellAtIndex(Escher::HighlightCell * cell, int j);
  void willDisplayExpressionCellAtIndex(Escher::HighlightCell * cell, int j) override;
  int modelIndexForRow(int j) override;
  int sequenceDefinitionForRow(int j);
  KDCoordinate maxFunctionNameWidth();
  void didChangeModelsList() override;
  KDCoordinate baseline(int j);
  void addModel() override;
  void editExpression(Ion::Events::Event event) override;
  bool removeModelRow(Ion::Storage::Record record) override;
  Shared::SequenceStore * modelStore() override;
  KDCoordinate nameWidth(int nameLength) const;

  Escher::SelectableTableView m_selectableTableView;
  Escher::EvenOddCell m_emptyCell;
  VerticalSequenceTitleCell m_sequenceTitleCells[k_maxNumberOfRows];
  Escher::EvenOddExpressionCell m_expressionCells[k_maxNumberOfRows];
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  Escher::StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
  KDCoordinate m_titlesColumnWidth;
  Escher::MemoizedRowHeightManager m_heightManager;
};

}

#endif
