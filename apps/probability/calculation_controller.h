#ifndef PROBABILITY_CALCULATION_CONTROLLER_H
#define PROBABILITY_CALCULATION_CONTROLLER_H

#include <escher.h>
#include "distribution/distribution.h"
#include "distribution_curve_view.h"
#include "calculation_cell.h"
#include "responder_image_cell.h"
#include "calculation/calculation.h"
#include "../shared/parameter_text_field_delegate.h"

namespace Probability {

class CalculationController : public ViewController, public TableViewDataSource, public SelectableTableViewDataSource, public Shared::ParameterTextFieldDelegate {
public:
  CalculationController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Distribution * distribution, Calculation * calculation);
  /* Responder */
  void didEnterResponderChain(Responder * previousResponder) override;
  void didBecomeFirstResponder() override;

  /* ViewController */
  View * view() override;
  const char * title() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  TELEMETRY_ID("Calculation");

  /* TableViewDataSource */
  int numberOfRows() const override;
  int numberOfColumns() const override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  /* TextField delegate */
  bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textSizeDidChange) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;

  void reloadDistributionCurveView();
  void reload();
  void setCalculationAccordingToIndex(int index, bool forceReinitialisation = false);
private:
  constexpr static int k_numberOfCalculationCells = 3;
  constexpr static KDCoordinate k_tableMargin = 3;
  void updateTitle();
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView, Distribution * distribution, Calculation * calculation);
    DistributionCurveView * distributionCurveView() {
      return &m_distributionCurveView;
    }
  private:
    constexpr static KDCoordinate k_titleHeightMargin = 5;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    MessageTextView m_titleView;
    SelectableTableView * m_selectableTableView;
    DistributionCurveView m_distributionCurveView;
  };
  ContentView m_contentView;
  SelectableTableView m_selectableTableView;
  ResponderImageCell m_imageCell;
  CalculationCell m_calculationCells[k_numberOfCalculationCells];
  Calculation * m_calculation;
  Distribution * m_distribution;
  constexpr static int k_titleBufferSize = 30;
  char m_titleBuffer[k_titleBufferSize];
};

}

#endif
