#ifndef APPS_PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H

#include "probability/abstract/dynamic_table_view_data_source.h"
#include "probability/abstract/table_view_controller.h"
#include "probability/gui/input_goodness_table_view.h"
#include "probability/models/statistic/goodness_statistic.h"

namespace Probability {

/* Controls the InputGoodnessTableView */
class GoodnessTableViewController : public TableViewController {
public:
  GoodnessTableViewController(Escher::Responder * parent,
                              GoodnessStatistic * statistic,
                              Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                              DynamicTableViewDataSourceDelegate * delegate,
                              Escher::SelectableTableViewDelegate * scrollDelegate);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  // TableViewController
  Escher::SelectableTableView * selectableTableView() override { return &m_inputTableView; }
  Escher::TableViewDataSource * tableViewDataSource() override { return &m_inputTableView; }

private:
  void deleteSelectedValue();
  GoodnessStatistic * m_statistic;
  InputGoodnessTableView m_inputTableView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H */
