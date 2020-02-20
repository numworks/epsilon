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
  View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Distribution");
  int numberOfRows() const override { return k_totalNumberOfModels; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate cellHeight() override { return k_cellHeight; }
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override { return k_numberOfCells; }
private:
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView) :
      m_titleView(KDFont::SmallFont, I18n::Message::ChooseDistribution, 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen),
      m_selectableTableView(selectableTableView)
    {}
    constexpr static KDCoordinate k_titleMargin = 8;
  private:
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    MessageTextView m_titleView;
    SelectableTableView * m_selectableTableView;
  };
  void setDistributionAccordingToIndex(int index);
  constexpr static KDCoordinate k_cellHeight = Metric::ParameterCellHeight;
  constexpr static int k_totalNumberOfModels = 9;
  constexpr static int k_numberOfCells = (Ion::Display::Height - Metric::TitleBarHeight - 14 - ContentView::k_titleMargin) /k_cellHeight + 1 + 1; // 14 for the small font height, + 1 to get the upper rounding and + 1 for half-displayed rows
  Cell m_cells[k_numberOfCells];
  SelectableTableView m_selectableTableView;
  ContentView m_contentView;
  I18n::Message * m_messages;
  Distribution * m_distribution;
  ParametersController * m_parametersController;

};

}

#endif
