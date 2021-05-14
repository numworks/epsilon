#ifndef HOMOGENEITY_RESULTS_CONTROLLER_H
#define HOMOGENEITY_RESULTS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>
#include <escher/view.h>
#include <kandinsky/coordinate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/gui/vertical_layout.h"

using namespace Escher;

namespace Probability {

class HomogeneityResultsView : public VerticalLayout<3>, public ButtonDelegate {
 public:
  HomogeneityResultsView(Responder * parent, SelectableTableView * table);
  void buttonAction() override;
  // TODO add selection behavior

 private:
  MessageTextView m_title;
  SelectableTableView * m_table;
  Shared::ButtonWithSeparator m_next;
};

class HomogeneityResultsDataSource : public TableViewDataSource {
 public:
  HomogeneityResultsDataSource();
  int numberOfRows() const override { return HomogeneityTableDataSource::k_initialNumberOfRows; }
  int numberOfColumns() const override { return HomogeneityTableDataSource::k_initialNumberOfColumns; }
  KDCoordinate columnWidth(int i) override { return HomogeneityTableDataSource::k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return HomogeneityTableDataSource::k_rowHeight; }
  int typeAtLocation(int i, int j) override { return 0; }
  HighlightCell * reusableCell(int i, int type) override;
  int reusableCellCount(int type) override { return numberOfRows() * numberOfColumns(); };

 private:
  EvenOddBufferTextCell m_cells[HomogeneityTableDataSource::k_maxNumberOfInnerCells];
};

class HomogeneityResultsController : public Page {
 public:
  HomogeneityResultsController(StackViewController * stackViewController);
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;

 private:
  HomogeneityResultsView m_contentView;
  HomogeneityTableDataSource m_tableData;
  HomogeneityResultsDataSource m_innerTableData;

  SelectableTableView m_table;
};

}  // namespace Probability

#endif /* HOMOGENEITY_RESULTS_CONTROLLER_H */
