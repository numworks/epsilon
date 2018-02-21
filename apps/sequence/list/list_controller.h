#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <escher.h>
#include "../sequence_title_cell.h"
#include "../sequence_store.h"
#include "../../shared/function_expression_cell.h"
#include "type_parameter_controller.h"
#include "../../shared/new_function_cell.h"
#include "../../shared/list_controller.h"
#include "../../shared/text_field_delegate.h"
#include "list_parameter_controller.h"
#include "sequence_toolbox.h"

namespace Sequence {

class ListController : public Shared::ListController, public Shared::TextFieldDelegate {
public:
  ListController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
  int numberOfRows() override;
  virtual KDCoordinate rowHeight(int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;
  void selectPreviousNewSequenceCell();
private:
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  void editExpression(Sequence * sequence, int sequenceDefinitionIndex, Ion::Events::Event event);
  ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  HighlightCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  int functionIndexForRow(int j) override;
  const char * textForRow(int j) override;
  int sequenceDefinitionForRow(int j);
  void addEmptyFunction() override;
  void editExpression(Shared::Function * function, Ion::Events::Event event) override;
  bool removeFunctionRow(Shared::Function * function) override;
  void reinitExpression(Shared::Function * function) override;
  View * loadView() override;
  void unloadView(View * view) override;
  static constexpr KDCoordinate k_emptySubRowHeight = 30;
  constexpr static int k_maxNumberOfRows = 3*MaxNumberOfSequences;
  SequenceStore * m_sequenceStore;
  SequenceTitleCell * m_sequenceTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell * m_expressionCells[k_maxNumberOfRows];
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
};

}

#endif
