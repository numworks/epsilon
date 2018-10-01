#include "list_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

//TODO static inline KDCoordinate max(KDCoordinate c1, KDCoordinate c2) { return c1 > c2 ? c1 : c2; }

namespace Sequence {

ListController::ListController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header, ButtonRowController * footer) :
  Shared::FunctionListController(parentResponder, sequenceStore, header, footer, I18n::Message::AddSequence),
  m_sequenceStore(sequenceStore),
  m_sequenceTitleCells{},
  m_expressionCells{},
  m_parameterController(this, sequenceStore),
  m_typeParameterController(this, sequenceStore, this, TableCell::Layout::Vertical),
  m_typeStackController(nullptr, &m_typeParameterController, KDColorWhite, Palette::PurpleDark, Palette::PurpleDark),
  m_sequenceToolbox()
{
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
}

const char * ListController::title() {
  return I18n::translate(I18n::Message::SequenceTab);
}

Toolbox * ListController::toolboxForTextInput(TextInput * textInput) {
  return toolboxForSender(textInput);
}

Toolbox * ListController::toolboxForLayoutField(LayoutField * layoutField) {
  return toolboxForSender(layoutField);
}

TextFieldDelegateApp * ListController::textFieldDelegateApp() {
  return (App *)app();
}

ExpressionFieldDelegateApp * ListController::expressionFieldDelegateApp() {
  return (App *)app();
}

int ListController::numberOfExpressionRows() {
  int numberOfRows = 0;
  for (int i = 0; i < m_sequenceStore->numberOfModels(); i++) {
    Sequence * sequence = m_sequenceStore->modelAtIndex(i);
    numberOfRows += sequence->numberOfElements();
  }
  if (m_sequenceStore->numberOfModels() == m_sequenceStore->maxNumberOfModels()) {
    return numberOfRows;
  }
  return 1 + numberOfRows;
};

KDCoordinate ListController::expressionRowHeight(int j) {
  if (m_sequenceStore->numberOfModels() < m_sequenceStore->maxNumberOfModels() && j == numberOfRows() - 1) {
    return Metric::StoreRowHeight;
  }
  Sequence * sequence = m_sequenceStore->modelAtIndex(modelIndexForRow(j));
  KDCoordinate defaultHeight = 2*k_expressionCellVerticalMargin + (sequence->type() == Sequence::Type::Explicit ? Metric::StoreRowHeight : k_emptySubRowHeight);
  Layout layout = sequence->layout();
  if (sequenceDefinitionForRow(j) == 1) {
    layout = sequence->firstInitialConditionLayout();
  }
  if (sequenceDefinitionForRow(j) == 2) {
    layout = sequence->secondInitialConditionLayout();
  }
  if (layout.isUninitialized()) {
    return defaultHeight;
  }
  KDCoordinate sequenceHeight = layout.layoutSize().height();
  return max(defaultHeight, sequenceHeight + 2*k_expressionCellVerticalMargin);
}

void ListController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::FunctionListController::willDisplayCellAtLocation(cell, i, j);
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(modelIndexForRow(j)%2 == 0);
}

void ListController::selectPreviousNewSequenceCell() {
  if (sequenceDefinitionForRow(selectedRow()) >= 0) {
    selectCellAtLocation(selectedColumn(), selectedRow()-sequenceDefinitionForRow(selectedRow()));
  }
}

Toolbox * ListController::toolboxForSender(Responder * sender) {
  // Set extra cells
  int recurrenceDepth = -1;
  int sequenceDefinition = sequenceDefinitionForRow(selectedRow());
  Sequence * sequence = m_sequenceStore->modelAtIndex(modelIndexForRow(selectedRow()));
  if (sequenceDefinition == 0) {
    recurrenceDepth = sequence->numberOfElements()-1;
  }
  m_sequenceToolbox.buildExtraCellsLayouts(sequence->name(), recurrenceDepth);
  // Set sender
  m_sequenceToolbox.setSender(sender);
  return &m_sequenceToolbox;
}

void ListController::editExpression(Sequence * sequence, int sequenceDefinition, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[TextField::maxBufferSize()];
  switch (sequenceDefinition) {
    case 0:
      strlcpy(initialTextContent, sequence->text(), sizeof(initialTextContent));
      break;
    case 1:
      strlcpy(initialTextContent, sequence->firstInitialConditionText(), sizeof(initialTextContent));
      break;
    default:
      strlcpy(initialTextContent, sequence->secondInitialConditionText(), sizeof(initialTextContent));
      break;
  }
  initialText = initialTextContent;
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  // Invalidate the sequences context cache
  static_cast<App *>(app())->localContext()->resetCache();
  switch (sequenceDefinition) {
    case 0:
      inputController->edit(this, event, sequence, initialText,
        [](void * context, void * sender){
        Sequence * mySequence = (Sequence *)context;
        InputViewController * myInputViewController = (InputViewController *)sender;
        const char * textBody = myInputViewController->textBody();
        mySequence->setContent(textBody);
        },
        [](void * context, void * sender){
      });
      break;
  case 1:
    inputController->edit(this, event, sequence, initialText,
      [](void * context, void * sender){
      Sequence * mySequence = (Sequence *)context;
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      mySequence->setFirstInitialConditionContent(textBody);
      },
      [](void * context, void * sender){
    });
    break;
  default:
    inputController->edit(this, event, sequence, initialText,
      [](void * context, void * sender){
      Sequence * mySequence = (Sequence *)context;
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      mySequence->setSecondInitialConditionContent(textBody);
      },
      [](void * context, void * sender){
    });
  }
}

bool ListController::removeModelRow(ExpressionModel * model) {
  Shared::FunctionListController::removeModelRow(model);
  // Invalidate the sequences context cache
  static_cast<App *>(app())->localContext()->resetCache();
  return true;
}

ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfRows() {
  return k_maxNumberOfRows;
}

HighlightCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_sequenceTitleCells[index];
}

HighlightCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  SequenceTitleCell * myCell = (SequenceTitleCell *)cell;
  Sequence * sequence = m_sequenceStore->modelAtIndex(modelIndexForRow(j));
  if (sequenceDefinitionForRow(j) == 0) {
    myCell->setLayout(sequence->definitionName());
  }
  if (sequenceDefinitionForRow(j) == 1) {
    myCell->setLayout(sequence->firstInitialConditionName());
  }
  if (sequenceDefinitionForRow(j) == 2) {
    myCell->setLayout(sequence->secondInitialConditionName());
  }
  KDColor nameColor = sequence->isActive() ? sequence->color() : Palette::GreyDark;
  myCell->setColor(nameColor);
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  Sequence * sequence = m_sequenceStore->modelAtIndex(modelIndexForRow(j));
  if (sequenceDefinitionForRow(j) == 0) {
    myCell->setLayout(sequence->layout());
  }
  if (sequenceDefinitionForRow(j) == 1) {
    myCell->setLayout(sequence->firstInitialConditionLayout());
  }
  if (sequenceDefinitionForRow(j) == 2) {
    myCell->setLayout(sequence->secondInitialConditionLayout());
  }
  bool active = sequence->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

int ListController::modelIndexForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (isAddEmptyRow(j)) {
    return modelIndexForRow(j-1)+1;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  do {
    sequenceIndex++;
    Sequence * sequence = m_sequenceStore->modelAtIndex(sequenceIndex);
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  return sequenceIndex;
}

bool ListController::isAddEmptyRow(int j) {
  return m_sequenceStore->numberOfModels() < m_sequenceStore->maxNumberOfModels() && j == numberOfRows() - 1;
}

int ListController::sequenceDefinitionForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (isAddEmptyRow(j)) {
    return 0;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  Sequence * sequence = nullptr;
  do {
    sequenceIndex++;
    sequence = m_sequenceStore->modelAtIndex(sequenceIndex);
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  return sequence->numberOfElements()-rowIndex+j;
}

void ListController::addEmptyModel() {
  app()->displayModalViewController(&m_typeStackController, 0.f, 0.f, Metric::TabHeight+Metric::ModalTopMargin, Metric::CommonRightMargin, Metric::ModalBottomMargin, Metric::CommonLeftMargin);
}

void ListController::editExpression(Shared::ExpressionModel * model, Ion::Events::Event event) {
  Sequence * sequence = static_cast<Sequence *>(model);
  editExpression(sequence, sequenceDefinitionForRow(selectedRow()), event);
}

void ListController::reinitExpression(Shared::ExpressionModel * model) {
  // Invalidate the sequences context cache
  static_cast<App *>(app())->localContext()->resetCache();
  Sequence * sequence = static_cast<Sequence *>(model);
  switch (sequenceDefinitionForRow(selectedRow())) {
    case 1:
      if (strlen(sequence->firstInitialConditionText()) == 0) {
        return;
      }
      sequence->setFirstInitialConditionContent("");
      break;
    case 2:
      if (strlen(sequence->secondInitialConditionText()) == 0) {
        return;
      }
      sequence->setSecondInitialConditionContent("");
      break;
    default:
      if (strlen(sequence->text()) == 0) {
        return;
      }
      sequence->setContent("");
      break;
  }
  selectableTableView()->reloadData();
}

}
