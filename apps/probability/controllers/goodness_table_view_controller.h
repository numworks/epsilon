#ifndef PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H

#include "probability/abstract/input_goodness_view.h"
#include "probability/abstract/dynamic_size_table_view_data_source.h"
#include "probability/abstract/table_view_controller.h"
#include "probability/gui/input_goodness_table_view.h"
#include "probability/models/statistic/goodness_test.h"

namespace Probability {

/* Controls the InputGoodnessTableView */
class GoodnessTableViewController : public TableViewController, public DynamicCellsDataSourceDelegate<EvenOddEditableTextCell> {
public:
  GoodnessTableViewController(Escher::Responder * parent,
                              GoodnessTest * statistic,
                              DynamicSizeTableViewDataSourceDelegate * delegate,
                              Escher::SelectableTableViewDelegate * scrollDelegate,
                              InputGoodnessView * inputGoodnessView);

  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  // TableViewController
  Escher::SelectableTableView * selectableTableView() override { return &m_inputTableView; }
  Escher::TableViewDataSource * tableViewDataSource() override { return &m_inputTableView; }

  void initCell(EvenOddEditableTextCell, void * cell, int index) override;
  // TODO factorize with HomogeneityTableViewController in TableViewController
  void recomputeDimensions() override;
  Escher::SelectableTableView * tableView() override { return &m_inputTableView; }

private:
  void deleteSelectedValue();
  GoodnessTest * m_statistic;
  InputGoodnessView * m_inputGoodnessView;
  InputGoodnessTableView m_inputTableView;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H */
