#ifndef SHARED_COLUMN_PARAMETER_CONTROLLER_H
#define SHARED_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include "editable_cell_table_view_controller.h"

namespace Shared {

class ColumnParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  static constexpr int k_titleBufferSize = Ion::Display::Width / 7; // KDFont::SmallFont->glyphSize().width() = 7
  ColumnParameterController(Escher::Responder * parentResponder) :
    SelectableListViewController(parentResponder),
    m_columnIndex(-1)
  {}
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  const char * title() override { return m_titleBuffer; };
  virtual void initializeColumnParameters(); // Always initialize parent class before initiliazing child.
protected:
  virtual EditableCellTableViewController * editableCellTableViewController() = 0;
  Escher::StackViewController * stackView();
  int m_columnIndex;
  char m_columnNameBuffer[EditableCellTableViewController::k_maxSizeOfColumnName];
  char m_titleBuffer[k_titleBufferSize];

};

}

#endif
