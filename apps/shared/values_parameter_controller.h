#ifndef SHARED_VALUES_PARAM_CONTROLLER_H
#define SHARED_VALUES_PARAM_CONTROLLER_H

#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/simple_list_view_data_source.h>
#include "interval_parameter_controller.h"

namespace Shared {
class ValuesParameterController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  ValuesParameterController(Escher::Responder * parentResponder);
  Escher::View * view() override;
  const char * title() override;
  void setPageTitle(I18n::Message pageTitle) { m_pageTitle = pageTitle; }
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate cellWidth() override {
    assert(m_selectableTableView.columnWidth(0) > 0);
    return m_selectableTableView.columnWidth(0);
  }
  // KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override { return 0; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
private:
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 3;
  Escher::MessageTableCellWithChevron m_copyColumn;
#else
  constexpr static int k_totalNumberOfCell = 2;
#endif
  I18n::Message m_pageTitle;
  Escher::MessageTableCell m_deleteColumn;
  Escher::MessageTableCellWithChevron m_setInterval;
  Escher::SelectableTableView m_selectableTableView;
};

}

#endif
