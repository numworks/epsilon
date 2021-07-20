#ifndef APPS_PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H

#include "probability/abstract/table_view_controller.h"
#include "probability/abstract/input_homogeneity_data_source.h"
#include "probability/models/statistic/homogeneity_statistic.h"

namespace Probability {

class HomogeneityTableViewController : public TableViewController {
public:
  HomogeneityTableViewController(Escher::Responder * parent,
                                 Escher::SelectableTableView * tableView,
                                 InputHomogeneityDataSource * dataSource,
                                 HomogeneityStatistic * statistic);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

private:
  Escher::SelectableTableView * m_contentView;
  InputHomogeneityDataSource * m_dataSource;
  HomogeneityStatistic * m_statistic;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H */
