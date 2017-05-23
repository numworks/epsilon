#ifndef PROBABILITY_LAW_CONTROLLER_H
#define PROBABILITY_LAW_CONTROLLER_H

#include <escher.h>
#include "cell.h"
#include "law/law.h"
#include "parameters_controller.h"
#include <new>

namespace Probability {

class LawController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  LawController(Responder * parentResponder, Law * m_law, ParametersController * parametersController);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
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
  void setLawAccordingToIndex(int index);
  constexpr static int k_totalNumberOfModels = 5;
  Cell m_cells[k_totalNumberOfModels];
  SelectableTableView m_selectableTableView;
  ContentView m_contentView;
  I18n::Message * m_messages;
  Law * m_law;
  ParametersController * m_parametersController;

};

}

#endif
