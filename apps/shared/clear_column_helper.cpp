#include "clear_column_helper.h"
#include "column_parameter_controller.h"
#include <escher/container.h>

using namespace Escher;

namespace Shared {

ClearColumnHelper::ClearColumnHelper() :
  m_confirmPopUpController(Invocation([](void * context, void * parent){
    ClearColumnHelper * param = static_cast<ClearColumnHelper *>(context);
    param->clearSelectedColumn();
    Container::activeApp()->dismissModalViewController();
    param->table()->reloadData();
    return true;
  }, this))
{}

void ClearColumnHelper::presentClearSelectedColumnPopupIfClearable() {
  int column = table()->selectedColumn();
  if (numberOfElementsInColumn(column) > 0 && isColumnClearable(column)) {
    setClearPopUpContent();
    m_confirmPopUpController.presentModally();
   }
}

void ClearColumnHelper::setClearPopUpContent() {
  char columnNameBuffer[ColumnParameterController::k_titleBufferSize];
  fillColumnName(table()->selectedColumn(), columnNameBuffer);
  m_confirmPopUpController.setMessageWithPlaceholder(I18n::Message::ClearColumnConfirmation, columnNameBuffer);
}


}
