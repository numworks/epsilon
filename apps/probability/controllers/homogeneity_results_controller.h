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

#include "../abstract/button_delegate.h"
#include "../gui/page_controller.h"
#include "../gui/solid_color_cell.h"
#include "../gui/vertical_layout.h"

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

class HomogeneityResultsController : public Page, public TableViewDataSource, public SelectableTableViewDataSource {
 public:
  HomogeneityResultsController(StackViewController * stackViewController);
  View * view() override { return &m_contentView; }

  // TableViewDataSource
  int numberOfRows() const override { return k_initialNumberOfRows; }
  int numberOfColumns() const override { return k_initialNumberOfColumns; }
  KDCoordinate columnWidth(int i) override { return k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }
  int typeAtLocation(int i, int j) { return 0; }
  HighlightCell * reusableCell(int i, int type) override;
  int reusableCellCount(int type) override { return numberOfRows() * numberOfColumns(); };

  int indexForEditableCell(int i);

 private:
  // TODO share all with InputHomogeneityController
  constexpr static int k_columnWidth = 80;
  constexpr static int k_rowHeight = 20;

  constexpr static int k_initialNumberOfRows = 5;
  constexpr static int k_initialNumberOfColumns = 4;
  constexpr static int k_maxNumberOfColumns = 6;
  constexpr static int k_maxNumberOfRows = 6;
  constexpr static int k_maxNumberOfResultCells = (k_maxNumberOfColumns - 1) * (k_maxNumberOfRows - 1);

  // TODO we can probably reuse the same DataSource as InputHomogeneityController
  SolidColorCell m_topLeftCell;
  EvenOddBufferTextCell m_rowHeader[k_maxNumberOfRows];
  EvenOddBufferTextCell m_colHeader[k_maxNumberOfColumns];
  EvenOddBufferTextCell m_cells[k_maxNumberOfResultCells];
  HomogeneityResultsView m_contentView;

  SelectableTableView m_table;
};

}  // namespace Probability

#endif /* HOMOGENEITY_RESULTS_CONTROLLER_H */
