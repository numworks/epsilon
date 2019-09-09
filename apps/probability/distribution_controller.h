#ifndef PROBABILITY_DISTRIBUTION_CONTROLLER_H
#define PROBABILITY_DISTRIBUTION_CONTROLLER_H

#include <escher.h>
#include "cell.h"
#include "distribution/distribution.h"
#include "parameters_controller.h"
#include <new>

namespace Probability {

class DistributionController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  DistributionController(Responder * parentResponder, Distribution * m_distribution, ParametersController * parametersController);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  int numberOfRows() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
private:
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView);
    constexpr static KDCoordinate k_titleMargin = 8;
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_titleView;;
    SelectableTableView * m_selectableTableView;
  };
  void setDistributionAccordingToIndex(int index);
  constexpr static int k_totalNumberOfModels = 8;
  Cell m_cells[k_totalNumberOfModels];
  SelectableTableView m_selectableTableView;
  ContentView m_contentView;
  I18n::Message * m_messages;
  Distribution * m_distribution;
  ParametersController * m_parametersController;

};

}

#endif
