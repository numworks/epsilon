#include "store_controller.h"

namespace Statistics::Categorical {

StoreController::StoreController(
    Escher::Responder* parentResponder, Escher::ButtonRowController* header,
    Escher::StackViewController* stackViewController,
    Escher::TabViewController* tabViewController,
    Escher::ViewController* dataTypeController, Store* store)
    : Shared::TabTableController(parentResponder),
      TableViewDataSource(&m_selectableTableView, this),
      Escher::ButtonRowDelegate(header, nullptr),
      m_dataTypeButton(this, I18n::Message::DataType,
                       Escher::Invocation::Builder<StoreController>(
                           [](StoreController* controller, void* sender) {
                             controller->pushTypeController();
                             return true;
                           },
                           this),
                       KDFont::Size::Small),
      m_deleteColumnConfirmPopUpController(
          Escher::Invocation::Builder<StoreController>(
              [](StoreController* param, void* parent) {
                param->clearSelectedColumn();
                Escher::App::app()->modalViewController()->dismissModal();
                param->recomputeDimensionsAndReload(true);
                return true;
              },
              this)),
      m_deleteRowConfirmPopUpController(
          Escher::Invocation::Builder<StoreController>(
              [](StoreController* param, void* parent) {
                param->clearSelectedRow();
                Escher::App::app()->modalViewController()->dismissModal();
                param->recomputeDimensionsAndReload(true);
                return true;
              },
              this)),
      m_selectableTableView(this, this, this, this),
      m_prefacedTableView(0, 0, this, &m_selectableTableView, this, this),
      m_store(store),
      m_columnParameterController(stackViewController, store,
                                  stackViewController, this),
      m_rfColumnParameterController(stackViewController, store,
                                    stackViewController),
      m_rowParameterController(stackViewController, store, stackViewController,
                               this),
      m_stackViewController(stackViewController),
      m_tabController(tabViewController),
      m_dataTypeController(dataTypeController) {
  m_prefacedTableView.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_prefacedTableView.setCellOverlap(0, 0);
  m_prefacedTableView.setMargins(k_margins);
}

bool StoreController::textFieldShouldFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
         (event == Ion::Events::Right &&
          m_selectableTableView.selectedColumn() < numberOfColumns() - 1) ||
         (event == Ion::Events::Left &&
          m_selectableTableView.selectedColumn() > 0) ||
         (event == Ion::Events::Down &&
          m_selectableTableView.selectedRow() < numberOfRows()) ||
         (event == Ion::Events::Up && m_selectableTableView.selectedRow() > 0);
}

bool StoreController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  double p = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(p)) {
    return false;
  }
  int row = m_selectableTableView.selectedRow(),
      col = m_selectableTableView.selectedColumn();
  if (!m_store->authorizedValue(p)) {
    Escher::App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  int dataCol = dataColumn(col);
  m_store->setValue(p, dataCol, dataRow(row));
  recomputeDimensionsAndReload(
      m_store->isRelativeFrequencyColumnActive(dataCol));
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    event = Ion::Events::Down;
  }
  m_selectableTableView.handleEvent(event);
  return true;
}

void StoreController::viewWillAppear() {
  if (selectedRow() < 0) {
    selectCellAtLocation(1, 1);
  }
  Shared::TabTableController::viewWillAppear();
}

void StoreController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0) {
      header()->setSelectedButton(0);
    } else {
      Shared::TabTableController::handleResponderChainEvent(event);
    }
  } else {
    Shared::TabTableController::handleResponderChainEvent(event);
  }
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (header()->selectedButton() >= 0) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
    } else {
      selectableTableView()->deselectTable();
      selectableTableView()->scrollToCell(0, 0);
      header()->setSelectedButton(0);
    }
    return true;
  }
  if (event == Ion::Events::Down &&
      (selectedRow() < 0 || selectedColumn() < 0)) {
    selectCellAtLocation(0, 0);
    header()->setSelectedButton(-1);
    Escher::App::app()->setFirstResponder(this);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int col = m_selectableTableView.selectedColumn(),
        row = m_selectableTableView.selectedRow();
    if (typeAtLocation(col, row) == k_typeOfInnerCells) {
      ColumnInfo info = columnInfo(col);
      if (info.isDataColumn) {
        m_store->eraseValue(info.groupNumber, dataRow(row));
        recomputeDimensionsAndReload(
            m_store->isRelativeFrequencyColumnActive(info.groupNumber));
      }
      return true;
    }
    if (typeAtLocation(col, row) == k_typeOfVerticalHeaderCells) {
      popupConfirmation(false, dataRow(row));
      return true;
    }
    if (typeAtLocation(col, row) == k_typeOfHeaderCells) {
      ColumnInfo info = columnInfo(col);
      if (info.isDataColumn) {
        popupConfirmation(true, info.groupNumber);
      }
      return info.isDataColumn;
    }
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == 0) {
      ColumnInfo info = columnInfo(selectedColumn());
      if (info.isDataColumn) {
        m_columnParameterController.setColumn(info.groupNumber);
        m_stackViewController->push(&m_columnParameterController);

      } else {
        m_rfColumnParameterController.setGroup(info.groupNumber);
        m_stackViewController->push(&m_rfColumnParameterController);
      }
      return true;
    } else if (selectedColumn() == 0) {
      m_rowParameterController.setRow(dataRow(selectedRow()));
      m_stackViewController->push(&m_rowParameterController);
      return true;
    }
    return false;
  }
  return false;
}

bool StoreController::recomputeDimensions() {
  constexpr int k_minGroupOrCategoryToShow = 2;
  TableDimension dim = m_store->currentDimension();
  int numberOfGroupsToShow =
      std::clamp(dim.col + 1, k_minGroupOrCategoryToShow,
                 Store::k_maxNumberOfGroups);  // +1 for empty col
  int numberOfCol = numberOfGroupsToShow + 1;  // vertical header
  for (int group = 0; group < numberOfGroupsToShow; ++group) {
    if (m_store->isRelativeFrequencyColumnActive(group)) {
      ++numberOfCol;
    }
  }
  // +2 for header + empty row
  dim.row = std::clamp(dim.row + 2, k_minGroupOrCategoryToShow + 1,
                       k_maxNumberOfRows);
  if (dim.row != m_numberOfRows || numberOfCol != m_numberOfColumns) {
    m_numberOfRows = dim.row;
    m_numberOfColumns = numberOfCol;
    return true;
  }
  return false;
}

void StoreController::recomputeDimensionsAndReload(bool force) {
  int col = m_selectableTableView.selectedColumn();
  int row = m_selectableTableView.selectedRow();
  if (recomputeDimensions() || force) {
    m_selectableTableView.reloadData(true);
  } else {
    m_selectableTableView.reloadCellAtLocation(col, row);
    // Reload header cell to update color
    m_selectableTableView.reloadCellAtLocation(col, 0);
    m_prefacedTableView.reloadPrefaceCellAtColumn(col);
  }
}

void StoreController::popupConfirmation(bool forColumn, int dataColOrRow) {
  char buffer[sizeof(Store::Label)];
  if (forColumn) {
    m_store->getGroupName(dataColOrRow, buffer, sizeof(buffer));
    m_deleteColumnConfirmPopUpController.setMessageWithPlaceholders(
        I18n::Message::ClearColumnConfirmation, buffer);
    m_deleteColumnConfirmPopUpController.presentModally();
  } else {
    m_store->getCategoryName(dataColOrRow, buffer, sizeof(buffer));
    m_deleteRowConfirmPopUpController.setMessageWithPlaceholders(
        I18n::Message::ClearRowConfirmation, buffer);
    m_deleteRowConfirmPopUpController.presentModally();
  }
}

}  // namespace Statistics::Categorical
