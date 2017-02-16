#include "list_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

ListController::ListController(Responder * parentResponder, SequenceStore * sequenceStore, HeaderViewController * header) :
  Shared::ListController(parentResponder, sequenceStore, header, "Ajouter une suite"),
  m_sequenceStore(sequenceStore),
  m_sequenceTitleCells{SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator),
    SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator),
    SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::VerticalIndicator)},
  m_parameterController(ListParameterController(this, sequenceStore)),
  m_typeParameterController(this, sequenceStore),
  m_typeStackController(StackViewController(nullptr, &m_typeParameterController, true, KDColorWhite, Palette::PurpleDark, Palette::PurpleDark)),
  m_sequenceToolbox(SequenceToolbox(m_sequenceStore))
{
}

const char * ListController::title() const {
  return "Suites";
}

Toolbox * ListController::toolboxForTextField(TextField * textField) {
  int recurrenceDepth = 0;
  int sequenceDefinition = sequenceDefinitionForRow(m_selectableTableView.selectedRow());
  Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndexForRow(m_selectableTableView.selectedRow()));
  if (sequenceDefinition == 0) {
    recurrenceDepth = sequence->numberOfElements()-1;
  }
  m_sequenceToolbox.addCells(recurrenceDepth);
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
    return k_emptyRowHeight;
  }
  Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndexForRow(j));
  KDCoordinate defaultHeight = sequence->type() == Sequence::Type::Explicite ? k_emptyRowHeight : k_emptySubRowHeight;
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
  return sequenceSize + defaultHeight - KDText::stringSize(" ").height();
}

void ListController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  Shared::ListController::willDisplayCellAtLocation(cell, i, j);
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(sequenceIndexForRow(j)%2 == 0);
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (Shared::ListController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::OK) {
    return handleEnter();
  }
  if ((!event.hasText() && event != Ion::Events::XNT)
      || m_selectableTableView.selectedColumn() == 0
      || m_selectableTableView.selectedRow() == numberOfRows() - 1) {
    return false;
  }
  Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndexForRow(m_selectableTableView.selectedRow()));
  editExpression(sequence, sequenceDefinitionForRow(m_selectableTableView.selectedRow()), event);
  return true;
}

bool ListController::handleEnter() {
  switch (m_selectableTableView.selectedColumn()) {
    case 0:
    {
      if (m_sequenceStore->numberOfFunctions() < m_sequenceStore->maxNumberOfFunctions() &&
          m_selectableTableView.selectedRow() == numberOfRows() - 1) {
        return true;
      }
      configureFunction(m_functionStore->functionAtIndex(sequenceIndexForRow(m_selectableTableView.selectedRow())));
      return true;
    }
    case 1:
    {
      if (m_sequenceStore->numberOfFunctions() < m_sequenceStore->maxNumberOfFunctions() &&
          m_selectableTableView.selectedRow() == numberOfRows() - 1) {
        app()->displayModalViewController(&m_typeStackController, 0.f, 0.f, 32, 20, 20, 20);
        return true;
      }
      Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndexForRow(m_selectableTableView.selectedRow()));
      editExpression(sequence, sequenceDefinitionForRow(m_selectableTableView.selectedRow()), Ion::Events::OK);
      return true;
    }
    default:
    {
      return false;
    }
  }
}

void ListController::editExpression(Sequence * sequence, int sequenceDefinition, Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[255];
  if (event == Ion::Events::OK) {
    strlcpy(initialTextContent, sequence->text(), sizeof(initialTextContent));
    initialText = initialTextContent;
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  inputController->setTextFieldDelegate(this);
  if (sequenceDefinition == 0) {
    inputController->edit(this, event, sequence, initialText,
      [](void * context, void * sender){
      Sequence * mySequence = (Sequence *)context;
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      mySequence->setContent(textBody);
      },
      [](void * context, void * sender){
    });
  }
  if (sequenceDefinition == 1) {
    inputController->edit(this, event, sequence, initialText,
      [](void * context, void * sender){
      Sequence * mySequence = (Sequence *)context;
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      mySequence->setFirstInitialConditionContent(textBody);
      },
      [](void * context, void * sender){
    });
  }
  if (sequenceDefinition == 2) {
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

ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfRows() {
  return k_maxNumberOfRows;
}

TableViewCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_sequenceTitleCells[index];
}

TableViewCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}


void ListController::willDisplayTitleCellAtIndex(TableViewCell * cell, int j) {
  SequenceTitleCell * myCell = (SequenceTitleCell *)cell;
  Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndexForRow(j));
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

void ListController::willDisplayExpressionCellAtIndex(TableViewCell * cell, int j) {
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  Sequence * sequence = m_sequenceStore->functionAtIndex(sequenceIndexForRow(j));
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

int ListController::sequenceIndexForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (m_sequenceStore->numberOfFunctions() < m_sequenceStore->maxNumberOfFunctions() &&
      j == numberOfRows() - 1) {
    return j;
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

}
