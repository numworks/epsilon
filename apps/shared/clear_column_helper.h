#ifndef SHARED_CLEAR_COLUMN_HELPER_H
#define SHARED_CLEAR_COLUMN_HELPER_H

#include "pop_up_controller.h"
#include <escher/selectable_table_view.h>

namespace Shared {

class ClearColumnHelper {
public:
  ClearColumnHelper();
  void presentClearSelectedColumnPopupIfClearable();
  virtual int fillColumnName(int columnIndex, char * buffer) = 0;

protected:
  virtual Escher::SelectableTableView * table() = 0;
  virtual int numberOfElementsInColumn(int columnIndex) const = 0;

  Shared::BufferPopUpController m_confirmPopUpController;

private:
  virtual void clearSelectedColumn() = 0;
  virtual void setClearPopUpContent();
  virtual bool isColumnClearable(int columnIndex) { return true; }
};

}

#endif
