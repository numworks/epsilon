#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/function_list_controller.h>
#include <apps/shared/sequence_store.h>
#include <escher/list_view_data_source.h>

#include "editable_sequence_cell.h"
#include "list_parameter_controller.h"
#include "sequence_cell.h"
#include "sequence_toolbox.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController : public Shared::FunctionListController,
                       public Escher::MemoizedListViewDataSource {
 public:
  ListController(Escher::Responder* parentResponder,
                 Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
                 Escher::ButtonRowController* header,
                 Escher::ButtonRowController* footer);
  const char* title() override {
    return I18n::translate(I18n::Message::SequenceTab);
  }
  int numberOfExpressionRows() const override;
  KDCoordinate expressionRowHeight(int j) override;
  Escher::Toolbox* toolbox() override;
  void selectPreviousNewSequenceCell();
  void editExpression(Ion::Events::Event event) override;
  /* ViewController */
  void viewWillAppear() override;
  /* TableViewDataSource */
  int numberOfRows() const override { return this->numberOfExpressionRows(); }
  int typeAtIndex(int index) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override {
    return type > k_expressionCellType ? 1 : maxNumberOfDisplayableRows();
  }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  /* ViewController */
  Escher::View* view() override { return &m_selectableTableView; }
  /* SelectableTableViewDelegate*/
  void tableViewDidChangeSelection(Escher::SelectableTableView* t,
                                   int previousSelectedCellX,
                                   int previousSelectedCellY,
                                   bool withinTemporarySelection) override;
  bool canStoreContentOfCellAtLocation(Escher::SelectableTableView* t, int col,
                                       int row) const override {
    return false;
  }
  /* ExpressionModelListController */
  Escher::SelectableTableView* selectableTableView() override {
    return &m_selectableTableView;
  }
  void showLastSequence();
  /* LayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;

 private:
  /* Cell types */
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_editableCellType = k_expressionCellType + 1;
  constexpr static int k_addModelCellType = k_editableCellType + 1;
  /* Model definitions */
  constexpr static int k_otherDefinition = -1;
  constexpr static int k_sequenceDefinition = 0;
  constexpr static int k_firstInitialCondition = k_sequenceDefinition + 1;
  constexpr static int k_secondInitialCondition = k_firstInitialCondition + 1;
  /* Width and margins */
  constexpr static KDCoordinate k_minTitleColumnWidth = 65;
  constexpr static KDCoordinate k_functionTitleSumOfMargins = 25;
  constexpr static KDCoordinate k_expressionCellVerticalMargin =
      Escher::Metric::BigCellMargin;
  /* Row numbers */
  constexpr static int k_maxNumberOfRows =
      3 * Shared::SequenceStore::k_maxNumberOfSequences;

  // Responder
  int initialColumnToSelect() const override { return 0; }

  // TableViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override {
    return expressionRowHeight(j);
  }
  void initCellSize(Escher::TableView* view) override {}

  // ExpressionModelListController
  void resetSizesMemoization() override { resetMemoization(); }

  void computeTitlesColumnWidth(bool forceMax = false);
  ListParameterController* parameterController() override {
    return &m_parameterController;
  }
  int maxNumberOfDisplayableRows() override { return k_maxNumberOfRows; }
  Escher::HighlightCell* titleCells(int index);
  Escher::HighlightCell* functionCells(int index) override;
  void willDisplayTitleCellAtIndex(Escher::HighlightCell* cell, int j);
  void willDisplayExpressionCellAtIndex(Escher::HighlightCell* cell,
                                        int j) override;
  int modelIndexForRow(int j) const override;
  int sequenceDefinitionForRow(int j) const;
  KDCoordinate maxFunctionNameWidth();
  void didChangeModelsList() override;
  KDCoordinate baseline(int j);
  void addModel() override;
  bool editSelectedRecordWithText(const char* text) override;
  void getTextForSelectedRecord(char* text, size_t size) const override;
  bool removeModelRow(Ion::Storage::Record record) override;
  Shared::SequenceStore* modelStore() const override;
  KDCoordinate nameWidth(int nameLength) const;
  Escher::ExpressionField* expressionField() override {
    return m_editableCell.expressionCell()->expressionField();
  }

  Escher::SelectableTableView m_selectableTableView;
  Escher::EvenOddCell m_emptyCell;
  SequenceCell m_sequenceCells[k_maxNumberOfRows];
  EditableSequenceCell m_editableCell;
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  Escher::StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
  KDCoordinate m_titlesColumnWidth;
  bool m_parameterColumnSelected;
};

}  // namespace Sequence

#endif
