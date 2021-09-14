#ifndef APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H

#include "probability/abstract/chained_selectable_table_view_delegate.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/abstract/results_homogeneity_data_source.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/results_homogeneity_view.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/gui/spacer_view.h"
#include "probability/models/statistic/homogeneity_statistic.h"
#include "results_controller.h"

namespace Probability {

class ResultsHomogeneityController : public Page,
                                     public ButtonDelegate,
                                     public Escher::SelectableTableViewDelegate {
public:
  ResultsHomogeneityController(StackViewController * stackViewController,
                               HomogeneityStatistic * statistic,
                               ResultsController * resultsController);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  const char * title() override { return nullptr; }
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool buttonAction() override;

  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

private:
  void selectCorrectView();
  void scrollToCorrectLocation();

  ResultsController * m_resultsController;
  ResultsHomogeneityView m_contentView;
  HomogeneityTableDataSourceWithTotals m_tableData;
  ResultsHomogeneityDataSource m_innerTableData;

  SelectableTableViewWithBackground m_table;
  bool m_isTableSelected;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H */
