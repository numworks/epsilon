#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <escher.h>
#include "../sequence_title_cell.h"
#include "../sequence_store.h"
#include "../../shared/model_expression_cell.h"
#include "../../shared/function_list_controller.h"
#include "../../shared/expression_layout_field_delegate.h"
#include "../../shared/text_field_delegate.h"
#include "list_parameter_controller.h"
#include "sequence_toolbox.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController : public Shared::FunctionListController, public Shared::TextFieldDelegate, public Shared::ExpressionLayoutFieldDelegate {
public:
  ListController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
  int numberOfExpressionRows() override;
  KDCoordinate expressionRowHeight(int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;
  Toolbox * toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) override;
  void selectPreviousNewSequenceCell();
  void editExpression(Sequence * sequence, int sequenceDefinitionIndex, Ion::Events::Event event);
private:
  Toolbox * toolboxForSender(Responder * sender);
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  Shared::ExpressionFieldDelegateApp * expressionFieldDelegateApp() override;
  ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  HighlightCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  int modelIndexForRow(int j) override;
  bool isAddEmptyRow(int j) override;
  int sequenceDefinitionForRow(int j);
  void addEmptyModel() override;
  void reinitExpression(Shared::ExpressionModel * model) override;
  void editExpression(Shared::ExpressionModel * model, Ion::Events::Event event) override;
  bool removeModelRow(Shared::ExpressionModel * model) override;
  View * loadView() override;
  void unloadView(View * view) override;
  static constexpr KDCoordinate k_emptySubRowHeight = 30;
  constexpr static int k_maxNumberOfRows = 3*MaxNumberOfSequences;
  SequenceStore * m_sequenceStore;
  SequenceTitleCell * m_sequenceTitleCells[k_maxNumberOfRows];
  Shared::ModelExpressionCell * m_expressionCells[k_maxNumberOfRows];
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
};

}

#endif
