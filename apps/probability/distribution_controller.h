#ifndef PROBABILITY_DISTRIBUTION_CONTROLLER_H
#define PROBABILITY_DISTRIBUTION_CONTROLLER_H

#include "cell.h"
#include "distribution/distribution.h"
#include "parameters_controller.h"
#include <new>

namespace Probability {

class DistributionController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  DistributionController(Escher::Responder * parentResponder, Distribution * m_distribution, ParametersController * parametersController);
  Escher::View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Distribution");
  int numberOfRows() const override { return k_totalNumberOfModels; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  KDCoordinate cellWidth() override {
    assert(m_selectableTableView.columnWidth(0) > 0);
    return m_selectableTableView.columnWidth(0);
  }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfCells; }
private:
  class ContentView : public Escher::View {
  public:
    ContentView(Escher::SelectableTableView * selectableTableView) :
      m_titleView(KDFont::SmallFont, I18n::Message::ChooseDistribution, 0.5f, 0.5f, Escher::Palette::GrayDark, Escher::Palette::WallScreen),
      m_selectableTableView(selectableTableView)
    {}
    constexpr static KDCoordinate k_titleMargin = 8;
  private:
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    Escher::MessageTextView m_titleView;
    Escher::SelectableTableView * m_selectableTableView;
  };
  void setDistributionAccordingToIndex(int index);
  constexpr static KDCoordinate k_cellHeight = Escher::Metric::ParameterCellHeight;
  constexpr static int k_totalNumberOfModels = 9;
  constexpr static int k_numberOfCells = 9; // (Ion::Display::Height - Escher::Metric::TitleBarHeight - 14 - ContentView::k_titleMargin) /k_cellHeight + 1 + 1; // 14 for the small font height, + 1 to get the upper rounding and + 1 for half-displayed rows
  Cell m_cells[k_numberOfCells];
  Escher::SelectableTableView m_selectableTableView;
  ContentView m_contentView;
  Distribution * m_distribution;
  ParametersController * m_parametersController;

};

}

#endif
