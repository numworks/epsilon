#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <escher/buffer_table_cell.h>
#include <escher/stack_view_controller.h>

#include "probability/gui/selectable_cell_list_controller.h"

namespace Probability {

class GraphController : public SelectableCellListPage<Escher::BufferTableCell, 1> {
 public:
  GraphController(StackViewController * stack) : SelectableCellListPage(stack) {
    m_cells[0].setLabelText("WAIT FOR IT...");
  }
  
};
}  // namespace Probability
#endif /* GRAPH_CONTROLLER_H */
