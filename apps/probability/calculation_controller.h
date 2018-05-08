#ifndef PROBABILITY_CALCULATION_CONTROLLER_H
#define PROBABILITY_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "law/law.h"
#include "law_curve_view.h"
#include "calculation_cell.h"
#include "responder_image_cell.h"
#include "calculation/calculation.h"
#include "../shared/parameter_text_field_delegate.h"

namespace Probability {

class CalculationController : public ViewController, public TableViewDataSource, public SelectableTableViewDataSource, public Shared::ParameterTextFieldDelegate {
public:
  CalculationController(Responder * parentResponder, Law * law, Calculation * calculation);
  /* Responder */
  void didEnterResponderChain(Responder * previousResponder) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  /* ViewController */
  View * view() override;
  const char * title() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  /* TableViewDataSource */
  int numberOfRows() override;
  int numberOfColumns() override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  /* TextField delegate */
  bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;

  void reloadLawCurveView();
  void reload();
  void setCalculationAccordingToIndex(int index, bool forceReinitialisation = false);
private:
  constexpr static int k_numberOfCalculationCells = 3;
  constexpr static KDCoordinate k_tableMargin = 3;
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  void updateTitle();
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView, Law * law, Calculation * calculation);
    LawCurveView * lawCurveView() {
      return &m_lawCurveView;
    }
  private:
    constexpr static KDCoordinate k_titleHeightMargin = 5;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_titleView;
    SelectableTableView * m_selectableTableView;
    LawCurveView m_lawCurveView;
  };
  ContentView m_contentView;
  SelectableTableView m_selectableTableView;
  ResponderImageCell m_imageCell;
  char m_draftTextBuffer[TextField::maxBufferSize()];
  CalculationCell m_calculationCells[k_numberOfCalculationCells];
  Calculation * m_calculation;
  Law * m_law;
  constexpr static int k_maxNumberOfTitleCharacters = 30;
  char m_titleBuffer[k_maxNumberOfTitleCharacters];
};

}

#endif
