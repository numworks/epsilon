#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <escher.h>
#include "../sequence_title_cell.h"
#include "../sequence_store.h"
#include "../../shared/function_expression_cell.h"
#include "../../shared/function_list_controller.h"
#include "../../shared/input_event_handler_delegate.h"
#include "../../shared/layout_field_delegate.h"
#include "../../shared/text_field_delegate.h"
#include "list_parameter_controller.h"
#include "sequence_toolbox.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController : public Shared::FunctionListController, public Shared::InputEventHandlerDelegate, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  ListController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, SequenceStore * sequenceStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
  int numberOfExpressionRows() override;
  KDCoordinate expressionRowHeight(int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  Toolbox * toolboxForInputEventHandler(InputEventHandler * handler) override;
  void selectPreviousNewSequenceCell();
  void editExpression(Sequence * sequence, int sequenceDefinitionIndex, Ion::Events::Event event);
private:
  static constexpr KDCoordinate k_expressionCellVerticalMargin = 3;
  Toolbox * toolboxForSender(InputEventHandler * sender);
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  Shared::ExpressionFieldDelegateApp * expressionFieldDelegateApp() override;
  Shared::InputEventHandlerDelegateApp * inputEventHandlerDelegateApp() override;
  ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
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
  static constexpr KDCoordinate k_emptySubRowHeight = 30;
  constexpr static int k_maxNumberOfRows = 3*MaxNumberOfSequences;
  SequenceStore * m_sequenceStore;
  SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
};

}

#endif
