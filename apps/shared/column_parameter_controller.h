#ifndef SHARED_COLUMN_PARAMETER_CONTROLLER_H
#define SHARED_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_list_view_controller.h>
#include "interval_parameter_controller.h"

namespace Shared {

static constexpr int k_lengthOfColumnName = 4;

class EditableCellTableViewController;

class ColumnParameterController : public Escher::SelectableListViewController {
public:
  ColumnParameterController(Escher::Responder * parentResponder);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  const char * title() override ;
  virtual void initializeColumnParameters();
protected:
  static constexpr int k_titleBufferSize = Ion::Display::Width / 7; // KDFont::SmallFont->glyphSize().width() = 7
  virtual EditableCellTableViewController * editableCellTableViewController() = 0;
  Escher::StackViewController * stackView();
  int m_columnIndex;
  char m_columnNameBuffer[k_lengthOfColumnName];
  char m_titleBuffer[k_titleBufferSize];

};

}

#endif
