#ifndef APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H

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
#include "probability/abstract/results_homogeneity_data_source.h"
#include "probability/gui/horizontal_or_vertical_layout.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/models/statistic/homogeneity_statistic.h"

using namespace Escher;

namespace Probability {

class HomogeneityResultsView : public VerticalLayout, public ButtonDelegate {
public:
  HomogeneityResultsView(Responder * parent, SelectableTableView * table);
  void buttonAction() override;
  int numberOfSubviews() const override { return 3; }
  Escher::View * subviewAtIndex(int i) override;
  // TODO add selection behavior

private:
  constexpr static int k_indexOfTitle = 0;
  constexpr static int k_indexOfTable = 1;
  constexpr static int k_indexOfButton = 2;

  MessageTextView m_title;
  SelectableTableView * m_table;
  Shared::ButtonWithSeparator m_next;
};

class ResultsHomogeneityController : public Page {
public:
  ResultsHomogeneityController(StackViewController * stackViewController,
                               HomogeneityStatistic * statistic);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitles;
  }
  const char * title() override { return "x2-test: Homogeneity/Independence"; }
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;

private:
  HomogeneityResultsView m_contentView;
  HomogeneityTableDataSource m_tableData;
  ResultsHomogeneityDataSource m_innerTableData;

  SelectableTableViewWithBackground m_table;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_RESULTS_HOMOGENEITY_CONTROLLER_H */
