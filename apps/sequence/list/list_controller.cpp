#include "list_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

ListController::ListController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header, ButtonRowController * footer) :
  Shared::ListController(parentResponder, sequenceStore, header, footer, I18n::Message::AddSequence),
  m_sequenceStore(sequenceStore),
  m_sequenceTitleCells{},
  m_expressionCells{},
  m_parameterController(this, sequenceStore),
  m_typeParameterController(this, sequenceStore, this, TableCell::Layout::Vertical),
  m_typeStackController(nullptr, &m_typeParameterController, KDColorWhite, Palette::PurpleDark, Palette::PurpleDark),
  m_sequenceToolbox()
{
}

const char * ListController::title() {
  return I18n::translate(I18n::Message::SequenceTab);
}

Toolbox * ListController::toolboxForTextInput(TextInput * textInput) {
  int recurrenceDepth = -1;
  int sequenceDefinition = sequenceDefinitionForRow(selectedRow());
  Sequence * sequence = m_sequenceStore->functionAtIndex(functionIndexForRow(selectedRow()));
  if (sequenceDefinition == 0) {
    recurrenceDepth = sequence->numberOfElements()-1;
  }
  m_sequenceToolbox.setExtraCells(sequence->name(), recurrenceDepth);
  return &m_sequenceToolbox;
}

TextFieldDelegateApp * ListController::textFieldDelegateApp() {
  return (App *)app();
}

int ListController::numberOfRows() {
  int numberOfRows = 0;
  for (int i = 0; i < m_sequenceStore->numberOfFunctions(); i++) {
    Sequence * sequence = m_sequenceStore->functionAtIndex(i);
    numberOfRows += sequence->numberOfElements();
  }
  if (m_sequenceStore->numberOfFunctions() == m_sequenceStore->maxNumberOfFunctions()) {
    return numberOfRows;
  }
  return 1 + numberOfRows;
};

KDCoordinate ListController::rowHeight(int j) {
  if (m_sequenceStore->numberOfFunctions() < m_sequenceStore->maxNumberOfFunctions() && j == numberOfRows() - 1) {
    return Metric::StoreRowHeight;
  }
  Sequence * sequence = m_sequenceStore->functionAtIndex(functionIndexForRow(j));
  KDCoordinate defaultHeight = sequence->type() == Sequence::Type::Explicit ? Metric::StoreRowHeight : k_emptySubRowHeight;
  ExpressionLayout * layout = sequence->layout();
  if (sequenceDefinitionForRow(j) == 1) {
    layout = sequence->firstInitialConditionLayout();
  }
  if (sequenceDefinitionForRow(j) == 2) {
    layout = sequence->secondInitialConditionLayout();
  }
  if (layout == nullptr) {
    return defaultHeight;
  }
  KDCoordinate sequenceSize = layout->size().height();
  return sequenceSize + defaultHeight - KDText::charSize().height();
}

void ListController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ListController::willDisplayCellAtLocation(cell, i, j);
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(functionIndexForRow(j)%2 == 0);
}

void ListController::selectPreviousNewSequenceCell() {
  if (sequenceDefinitionForRow(selectedRow()) >= 0) {
    selectCellAtLocation(selectedColumn(), selectedRow()-sequenceDefinitionForRow(selectedRow()));
  }
}

void ListController::editExpression(Sequence * sequence, int sequenceDefinition, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[TextField::maxBufferSize()];
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
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
  }
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

bool ListController::removeFunctionRow(Function * function) {
  m_functionStore->removeFunction(function);
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
  return m_sequenceTitleCells[index];
}

HighlightCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return m_expressionCells[index];
}


void ListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  SequenceTitleCell * myCell = (SequenceTitleCell *)cell;
  Sequence * sequence = m_sequenceStore->functionAtIndex(functionIndexForRow(j));
  if (sequenceDefinitionForRow(j) == 0) {
    myCell->setExpression(sequence->definitionName());
  }
  if (sequenceDefinitionForRow(j) == 1) {
    myCell->setExpression(sequence->firstInitialConditionName());
  }
  if (sequenceDefinitionForRow(j) == 2) {
    myCell->setExpression(sequence->secondInitialConditionName());
  }
  KDColor nameColor = sequence->isActive() ? sequence->color() : Palette::GreyDark;
  myCell->setColor(nameColor);
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  Sequence * sequence = m_sequenceStore->functionAtIndex(functionIndexForRow(j));
  if (sequenceDefinitionForRow(j) == 0) {
    myCell->setExpression(sequence->layout());
  }
  if (sequenceDefinitionForRow(j) == 1) {
    myCell->setExpression(sequence->firstInitialConditionLayout());
  }
  if (sequenceDefinitionForRow(j) == 2) {
    myCell->setExpression(sequence->secondInitialConditionLayout());
  }
  bool active = sequence->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

int ListController::functionIndexForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (m_sequenceStore->numberOfFunctions() < m_sequenceStore->maxNumberOfFunctions() &&
      j == numberOfRows() - 1) {
    return functionIndexForRow(j-1)+1;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  do {
    sequenceIndex++;
    Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndex);
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  return sequenceIndex;
}

const char * ListController::textForRow(int j) {
  Sequence * sequence = (Sequence *)m_functionStore->functionAtIndex(functionIndexForRow(j));
  switch (sequenceDefinitionForRow(j)) {
    case 0:
     return sequence->text();
   case 1:
     return sequence->firstInitialConditionText();
   case 2:
     return sequence->secondInitialConditionText();
   default:
     assert(false);
     return nullptr;
  }
}

int ListController::sequenceDefinitionForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (m_sequenceStore->numberOfFunctions() < m_sequenceStore->maxNumberOfFunctions() &&
      j == numberOfRows() - 1) {
    return 0;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  Sequence * sequence = nullptr;
  do {
    sequenceIndex++;
    sequence = m_sequenceStore->functionAtIndex(sequenceIndex);
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  return sequence->numberOfElements()-rowIndex+j;
}

void ListController::addEmptyFunction() {
  app()->displayModalViewController(&m_typeStackController, 0.f, 0.f, Metric::TabHeight+Metric::ModalTopMargin, Metric::CommonRightMargin, Metric::ModalBottomMargin, Metric::CommonLeftMargin);
}

void ListController::editExpression(Shared::Function * function, Ion::Events::Event event) {
  Sequence * sequence = (Sequence *)function;
  editExpression(sequence, sequenceDefinitionForRow(selectedRow()), event);
}

void ListController::reinitExpression(Shared::Function * function) {
  // Invalidate the sequences context cache
  static_cast<App *>(app())->localContext()->resetCache();
  Sequence * sequence = (Sequence *)function;
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

View * ListController::loadView() {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_sequenceTitleCells[i] = new SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator);
    m_expressionCells[i] = new FunctionExpressionCell();
  }
  return Shared::ListController::loadView();
}

void ListController::unloadView(View * view) {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    delete m_sequenceTitleCells[i];
    m_sequenceTitleCells[i] = nullptr;
    delete m_expressionCells[i];
    m_expressionCells[i] = nullptr;
  }
  Shared::ListController::unloadView(view);
}

}
