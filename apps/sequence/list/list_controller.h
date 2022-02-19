#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <escher.h>
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

class ListController : public Shared::FunctionListController, public Shared::InputEventHandlerDelegate, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  ListController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
  int numberOfExpressionRows() const override;
  KDCoordinate expressionRowHeight(int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  Toolbox * toolboxForInputEventHandler(InputEventHandler * handler) override;
  void selectPreviousNewSequenceCell();
  void editExpression(int sequenceDefinitionIndex, Ion::Events::Event event);
protected:
  virtual const char * recordExtension() const override { return Ion::Storage::seqExtension; }
private:
  static constexpr KDCoordinate k_expressionCellVerticalMargin = 3;
  bool editInitialConditionOfSelectedRecordWithText(const char * text, bool firstInitialCondition);
  ListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  int modelIndexForRow(int j) override;
  int sequenceDefinitionForRow(int j);
  void addEmptyModel() override;
  void reinitSelectedExpression(Shared::ExpiringPointer<Shared::ExpressionModelHandle> model) override;
  void editExpression(Ion::Events::Event event) override;
  bool removeModelRow(Ion::Storage::Record record) override;
  Shared::SequenceStore * modelStore() override;
  constexpr static int k_maxNumberOfRows = 3*Shared::MaxNumberOfSequences;
  Shared::SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  ListParameterController m_parameterController;
  TypeParameterController m_typeParameterController;
  StackViewController m_typeStackController;
  SequenceToolbox m_sequenceToolbox;
};

}

#endif
