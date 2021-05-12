#ifndef INPUT_HOMOGENEITY_CONTROLLER_H
#define INPUT_HOMOGENEITY_CONTROLLER_H

#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/solid_color_view.h>
#include <escher/table_view_data_source.h>
#include <kandinsky/color.h>
#include <kandinsky/coordinate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/input_table_view.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/solid_color_cell.h"
#include "homogeneity_results_controller.h"

namespace Probability {

class InputHomogeneityDataSource : public TableViewDataSource {
 public:
  InputHomogeneityDataSource(Responder * parentResponder, SelectableTableView * tableView,
                             InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate);
  int numberOfRows() const override { return k_initialNumberOfRows; }
  int numberOfColumns() const override { return k_initialNumberOfColumns; }
  int reusableCellCount(int type) override { return numberOfRows() * numberOfColumns(); }
  int typeAtLocation(int i, int j) { return 0; }
  HighlightCell * reusableCell(int i, int type) override;

  KDCoordinate columnWidth(int i) override { return k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

 private:
  int indexForEditableCell(int i);

  // TODO needed ?
  constexpr static int k_columnWidth = 80;
  constexpr static int k_rowHeight = 20;

  constexpr static int k_initialNumberOfRows = 5;
  constexpr static int k_initialNumberOfColumns = 4;
  constexpr static int k_maxNumberOfColumns = 6;
  constexpr static int k_maxNumberOfRows = 6;
  constexpr static int k_maxNumberOfEditableCells = (k_maxNumberOfColumns - 1) * (k_maxNumberOfRows - 1);

  SolidColorCell m_topLeftCell;
  EvenOddBufferTextCell m_rowHeader[k_maxNumberOfRows];
  EvenOddBufferTextCell m_colHeader[k_maxNumberOfColumns];
  // TODO is it worth reusing cells ?
  EvenOddEditableTextCell m_cells[k_maxNumberOfEditableCells];
};

class InputHomogeneityController : public Page, public ButtonDelegate {
 public:
  InputHomogeneityController(StackViewController * parent, HomogeneityResultsController * homogeneityResultsController,
                             InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate);
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  void buttonAction() override;

 private:
  InputHomogeneityDataSource m_data;
  InputTableView m_contentView;
  SelectableTableView m_table;

  HomogeneityResultsController * m_homogeneityResultsController;
};

}  // namespace Probability

#endif /* INPUT_HOMOGENEITY_CONTROLLER_H */
