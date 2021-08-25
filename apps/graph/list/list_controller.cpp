#include "list_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>
#include <escher/metric.h>
#include <apps/apps_container.h>
#include <poincare/code_point_layout.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

ListController::ListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  Shared::FunctionListController(parentResponder, header, footer, I18n::Message::AddFunction),
  m_functionTitleCells{ //TODO find better initialization
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
  },
  m_expressionCells{},
  m_parameterController(this, this, I18n::Message::FunctionColor, I18n::Message::DeleteFunction, inputEventHandlerDelegate),
  m_modelsParameterController(this, nullptr, this),
  m_modelsStackController(nullptr, &m_modelsParameterController)
{
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
}

const char * ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

void ListController::renameSelectedFunction() {
  assert(selectedColumn() == 0);
  assert(selectedRow() >= 0 && selectedRow() < numberOfRows()-1); // TODO change if sometimes the addFunction row is not displayed

  // Increase the size of the name column
  computeTitlesColumnWidth(true);
  selectableTableView()->reloadData();

  AppsContainer::sharedAppsContainer()->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::AlphaLock);
  TextFieldFunctionTitleCell * selectedTitleCell = (TextFieldFunctionTitleCell *)(selectableTableView()->selectedCell());
  selectedTitleCell->setHorizontalAlignment(1.0f);
  Container::activeApp()->setFirstResponder(selectedTitleCell);
  selectedTitleCell->setEditing(true);
}

bool ListController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  assert(textField != nullptr);
  assert(false); // TODO Hugo : check;
  return true;
  // // Compute the new name
  // size_t textLength = strlen(text);
  // size_t argumentLength = UTF8Helper::HasCodePoint(text, UCodePointGreekSmallLetterTheta) ? NewFunction::k_parenthesedThetaArgumentByteLength : NewFunction::k_parenthesedXNTArgumentByteLength;
  // constexpr int maxBaseNameSize = NewFunction::k_maxNameWithArgumentSize;
  // char baseName[maxBaseNameSize];
  // if (textLength <= argumentLength) {
  //   // The user entered an empty name. Use a default function name.
  //   NewFunction::DefaultName(baseName, maxBaseNameSize);
  //   /* We don't need to update the textfield edited text here because we are
  //    * sure that the default name is compliant. It will thus lead to the end of
  //    * edition and its content will be reloaded by willDisplayTitleCellAtIndex. */
  // } else {
  //   assert(argumentLength <= textLength + 1);
  //   strlcpy(baseName, text, textLength - argumentLength + 1);
  // }

  // // Delete any variable with the same name
  // GlobalContext::DestroyRecordsBaseNamedWithoutExtension(baseName, Ion::Storage::funcExtension);

  // // Set the name
  // NewFunction::NameNotCompliantError nameError = NewFunction::BaseNameCompliant(baseName);
  // Ion::Storage::Record::ErrorStatus error = nameError == NewFunction::NameNotCompliantError::None ? modelStore()->recordAtIndex(m_selectableTableView.selectedRow()).setBaseNameWithExtension(baseName, Ion::Storage::funcExtension) : Ion::Storage::Record::ErrorStatus::NonCompliantName;

  // // Handle any error
  // if (error == Ion::Storage::Record::ErrorStatus::None) {
  //   bool selectTab = false;
  //   textField->setEditing(false);
  //   computeTitlesColumnWidth();
  //   int currentRow = m_selectableTableView.selectedRow();
  //   if (event == Ion::Events::Down && currentRow < numberOfRows() - 1) {
  //     m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), currentRow + 1);
  //   } else if (event == Ion::Events::Up) {
  //     if (currentRow > 0) {
  //       m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), currentRow - 1);
  //     } else {
  //       selectTab = true;
  //     }
  //   }
  //   m_selectableTableView.selectedCell()->setHighlighted(true);
  //   m_selectableTableView.reloadData();
  //   Container::activeApp()->setFirstResponder(&m_selectableTableView);
  //   if (selectTab) {
  //     m_selectableTableView.parentResponder()->handleEvent(event);
  //   }
  //   AppsContainer::sharedAppsContainer()->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  //   return true;
  // } else if (error == Ion::Storage::Record::ErrorStatus::NameTaken) {
  //   Container::activeApp()->displayWarning(I18n::Message::NameTaken);
  // } else if (error == Ion::Storage::Record::ErrorStatus::NonCompliantName) {
  //   assert(nameError != NewFunction::NameNotCompliantError::None);
  //   if (nameError == NewFunction::NameNotCompliantError::CharacterNotAllowed) {
  //     Container::activeApp()->displayWarning(I18n::Message::AllowedCharactersAZaz09);
  //   } else if (nameError == NewFunction::NameNotCompliantError::NameCannotStartWithNumber) {
  //     Container::activeApp()->displayWarning(I18n::Message::NameCannotStartWithNumber);
  //   } else if (nameError == NewFunction::NameNotCompliantError::NameCannotStartWithUnderscore) {
  //     Container::activeApp()->displayWarning(I18n::Message::NameCannotStartWithUnderscore);
  //   } else {
  //     assert(nameError == NewFunction::NameNotCompliantError::ReservedName);
  //     Container::activeApp()->displayWarning(I18n::Message::ReservedName);
  //   }
  // } else {
  //   assert(error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
  //   Container::activeApp()->displayWarning(I18n::Message::NameTooLong);
  // }
  // textField->setEditing(true);
  // return false;
}

bool ListController::textFieldDidAbortEditing(TextField * textField) {
  assert(textField != nullptr);
  // Put the name column back to normal size
  computeTitlesColumnWidth();
  selectableTableView()->reloadData();
  ExpiringPointer<NewFunction> function = modelStore()->modelForRecord(modelStore()->recordAtIndex(selectedRow()));
  setFunctionNameInTextField(function, textField);
  m_selectableTableView.selectedCell()->setHighlighted(true);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
  AppsContainer::sharedAppsContainer()->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  return true;
}

bool ListController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::Up || event == Ion::Events::Down || Shared::TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool layoutRepresentsAnEquality(Poincare::Layout l) {
  Poincare::Layout match = l.recursivelyMatches(
      [](Poincare::Layout layout) {
      CodePoint symbols[5] = { '=', '<', '>', UCodePointInferiorEqual, UCodePointSuperiorEqual};
      for (size_t i = 0; i < sizeof(symbols); i++) {
        if (layout.type() == Poincare::LayoutNode::Type::CodePointLayout && static_cast<Poincare::CodePointLayout &>(layout).codePoint() == symbols[i]) {
          return true;
        }
      }
      return false;
    });
  return !match.isUninitialized();
}

bool ListController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  assert(textField != nullptr);
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    return false;
  }
  return Shared::TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

bool ListController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutRepresentsAnEquality(layoutField->layout())) {
      layoutField->putCursorLeftOfLayout();
      layoutField->handleEventWithText("y=");
    }
  }
  return Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event);
}

bool ListController::layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) {
  return true;
}

Shared::ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfDisplayableRows;
}

FunctionTitleCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_functionTitleCells[index];
}

HighlightCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
}

HighlightCell * ListController::parameterCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionParameterCells[index];
}

void ListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  assert(cell != nullptr);
  assert(j >= 0 && j < modelStore()->numberOfModels());
  TextFieldFunctionTitleCell * titleCell = static_cast<TextFieldFunctionTitleCell *>(cell);
  // Update the corresponding expression cell in order to get the baseline
  ExpressionModelListController::willDisplayExpressionCellAtIndex(m_selectableTableView.cellAtLocation(1, j), j);
  titleCell->setBaseline(baseline(j));
  if (!titleCell->isEditing()) {
    // Set name and color if the name is not being edited
    ExpiringPointer<NewFunction> function = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
    setFunctionNameInTextField(function, titleCell->textField());
    KDColor functionNameColor = function->isActive() ? function->color() : Palette::GrayDark;
    titleCell->setColor(functionNameColor);
  }
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  assert(cell != nullptr);
  assert(j >= 0 && j < modelStore()->numberOfModels());
  Shared::FunctionListController::willDisplayExpressionCellAtIndex(cell, j);
  FunctionExpressionCell * myCell = static_cast<FunctionExpressionCell *>(cell);
  ExpiringPointer<NewFunction> f = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  KDColor textColor = f->isActive() ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
}

void ListController::willDisplayParameterCellAtIndex(HighlightCell * cell, int j) {
  assert(cell != nullptr);
  assert(j >= 0 && j < modelStore()->numberOfModels());
}

void ListController::setFunctionNameInTextField(ExpiringPointer<NewFunction> function, TextField * textField) {
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(&m_modelsStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

ContinuousFunctionStore * ListController::modelStore() {
  return App::app()->functionStore();
}

}
