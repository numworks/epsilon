#ifndef APPS_PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H

#include "probability/abstract/dynamic_data_source.h"
#include "probability/abstract/table_view_controller.h"
#include "probability/gui/input_goodness_table_view.h"
#include "probability/models/statistic/goodness_statistic.h"

namespace Probability {

/* Controls the InputGoodnessTableView */
class GoodnessTableViewController : public TableViewController {
public:
  GoodnessTableViewController(Escher::Responder * parent,
                              GoodnessStatistic * statistic,
                              InputGoodnessTableView * dataSource);
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

private:
  GoodnessStatistic * m_statistic;
  InputGoodnessTableView * m_dataSource;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_GOODNESS_TABLE_VIEW_CONTROLLER_H */
