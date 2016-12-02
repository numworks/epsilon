#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>
#include "law.h"

namespace Probability {

class ParametersController : public ViewController, public SimpleListViewDataSource {
public:
  ParametersController(Responder * parentResponder, Law * law);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfRows() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, SelectableTableView * selectableTableView);
    Button * button();
    PointerTextView * parameterDefinitionAtIndex(int index);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void layoutSubviews() override;
  private:
    constexpr static KDCoordinate k_buttonHeight = 40;
    constexpr static KDCoordinate k_textHeight = 30;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    Button m_nextButton;
    PointerTextView m_firstParameterDefinition;
    PointerTextView m_secondParameterDefinition;
    SelectableTableView * m_selectableTableView;
  };
  TextMenuListCell m_menuListCell[2];
  SelectableTableView m_selectableTableView;
  ContentView m_contentView;
  Law * m_law;
  bool m_buttonSelected;
};

}

#endif
