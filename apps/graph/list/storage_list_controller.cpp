#include "storage_list_controller.h"
#include "../app.h"
#include "../../i18n.h"
#include <assert.h>
#include <escher/metric.h>

using namespace Shared;

namespace Graph {

StorageListController::StorageListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer) :
  Shared::StorageFunctionListController(parentResponder, header, footer, I18n::Message::AddFunction),
  m_functionTitleCells{ //TODO find better initialization
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
    TextFieldFunctionTitleCell(this),
  },
  m_expressionCells{},
  m_parameterController(this, this, I18n::Message::FunctionColor, I18n::Message::DeleteFunction)
{
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
}

const char * StorageListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

void StorageListController::renameSelectedFunction() {
  assert(selectedColumn() == 0);
  assert(selectedRow() >= 0 && selectedRow() < numberOfRows()-1); // TODO change if sometimes the addFunction row is not displayed
  TextFieldFunctionTitleCell * selectedTitleCell = (TextFieldFunctionTitleCell *)(selectableTableView()->selectedCell());
  app()->setFirstResponder(selectedTitleCell);
  selectedTitleCell->setEditing(true);
}

bool StorageListController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  // Compute the new name
  size_t textLength = strlen(text);
  size_t argumentLength = StorageFunction::k_parenthesedArgumentLength;
  constexpr int maxBaseNameSize = StorageFunction::k_maxNameWithArgumentSize;
  char baseName[maxBaseNameSize];
  if (textLength <= argumentLength) {
    // The user entered an empty name. Use a default function name.
    StorageCartesianFunction::DefaultName(baseName, maxBaseNameSize);
  } else {
    strlcpy(baseName, text, textLength - argumentLength + 1);
  }

  // Set the name
  Ion::Storage::Record::ErrorStatus error = StorageCartesianFunction::baseNameCompliant(baseName) ? modelStore()->recordAtIndex(m_selectableTableView.selectedRow()).setBaseNameWithExtension(baseName, GlobalContext::funcExtension /*TODO store elsewhere?*/) : Ion::Storage::Record::ErrorStatus::NonCompliantName;

  // Handle any error
  if (error == Ion::Storage::Record::ErrorStatus::None) {
    computeTitlesColumnWidth();
    m_selectableTableView.selectedCell()->setHighlighted(true);
    m_selectableTableView.reloadData();
    app()->setFirstResponder(&m_selectableTableView);
    return true;
  } else if (error == Ion::Storage::Record::ErrorStatus::NameTaken) {
    app()->displayWarning(I18n::Message::NameTaken);
  } else if (error == Ion::Storage::Record::ErrorStatus::NonCompliantName) {
    app()->displayWarning(I18n::Message::NonCompliantName);
  } else {
    assert(error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    app()->displayWarning(I18n::Message::NameTooLong);
  }
  return false;
}

bool StorageListController::textFieldDidAbortEditing(TextField * textField) {
  StorageFunction * function = modelStore()->modelForRecord(modelStore()->recordAtIndex(selectedRow()));
  setFunctionNameInTextField(function, textField);
  m_selectableTableView.selectedCell()->setHighlighted(true);
  app()->setFirstResponder(&m_selectableTableView);
  return true;
}

bool StorageListController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::Up || event == Ion::Events::Down || Shared::TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

StorageListParameterController * StorageListController::parameterController() {
  return &m_parameterController;
}

int StorageListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfDisplayableRows;
}

FunctionTitleCell * StorageListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_functionTitleCells[index];
}

HighlightCell * StorageListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
}

void StorageListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  TextFieldFunctionTitleCell * titleCell = static_cast<TextFieldFunctionTitleCell *>(cell);
  StorageFunction * function = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  setFunctionNameInTextField(function, titleCell->textField());
  KDColor functionNameColor = function->isActive() ? function->color() : Palette::GreyDark;
  titleCell->setColor(functionNameColor);
}

void StorageListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  Shared::StorageFunctionListController::willDisplayExpressionCellAtIndex(cell, j);
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  StorageFunction * f = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  KDColor textColor = f->isActive() ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

void StorageListController::setFunctionNameInTextField(StorageFunction * function, TextField * textField) {
  char bufferName[BufferTextView::k_maxNumberOfChar];
  function->nameWithArgument(bufferName, BufferTextView::k_maxNumberOfChar, modelStore()->symbol());
  textField->setText(bufferName);
}

}
