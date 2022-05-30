#ifndef INFERENCE_PROBABILITY_CALCULATION_CONTROLLER_H
#define INFERENCE_PROBABILITY_CALCULATION_CONTROLLER_H

#include <apps/shared/parameter_text_field_delegate.h>

#include "inference/constants.h"
#include "inference/probability/calculation_cell.h"
#include "inference/probability/calculation_popup_data_source.h"
#include "inference/probability/distribution_curve_view.h"
#include <escher/dropdown_view.h>
#include <escher/view_controller.h>
#include <escher/stack_view_controller.h>
#include "inference/models/probability/calculation/calculation.h"
#include "inference/models/probability/distribution/distribution.h"
#include "inference/text_helpers.h"

namespace Inference {

class CalculationController : public Escher::ViewController,
                              public Escher::TableViewDataSource,
                              public Escher::SelectableTableViewDataSource,
                              public Shared::ParameterTextFieldDelegate,
                              public Escher::DropdownCallback {
public:
  CalculationController(Escher::StackViewController * parentResponder,
                        Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                        Distribution * distribution,
                        Calculation * calculation);

  void reinitCalculation();

  /* Responder */
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  /* ViewController */
  Escher::View * view() override { return &m_contentView; }
  const char * title() override { return m_titleBuffer; }
  TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  TELEMETRY_ID("Calculation");

  /* TableViewDataSource */
  int numberOfRows() const override { return 1; }
  int numberOfColumns() const override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return 1; }
  int typeAtLocation(int i, int j) override { return i; }
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  /* TextField delegate */
  bool textFieldDidHandleEvent(Escher::TextField * textField,
                               bool returnValue,
                               bool textSizeDidChange) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  void reloadDistributionCurveView();
  void reload();

  // Escher::Dropdown
  void onDropdownSelected(int selectedRow) override;
  bool popupDidReceiveEvent(Ion::Events::Event event, Escher::Responder * responder) override;

private:
  constexpr static int k_numberOfCalculationCells = 3;
  constexpr static KDCoordinate k_tableMargin = 3;
  void updateTitle();
  void setCalculationAccordingToIndex(int index, bool forceReinitialisation = false);
  class ContentView : public Escher::View {
  public:
    ContentView(Escher::SelectableTableView * selectableTableView,
                Distribution * distribution,
                Calculation * calculation);
    DistributionCurveView * distributionCurveView() { return &m_distributionCurveView; }

  private:
    int numberOfSubviews() const override { return 2; };
    Escher::View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    Escher::SelectableTableView * m_selectableTableView;
    DistributionCurveView m_distributionCurveView;
  };
  Calculation * m_calculation;
  Distribution * m_distribution;
  ContentView m_contentView;
  Escher::SelectableTableView m_selectableTableView;
  CalculationPopupDataSource m_imagesDataSource;
  Escher::Dropdown m_dropdown;
  CalculationCell m_calculationCells[k_numberOfCalculationCells];
  constexpr static int k_titleBufferSize = sizeof("d1 =  d2 =  ") +
                                           2 * Constants::k_shortFloatNumberOfChars;
  char m_titleBuffer[k_titleBufferSize];
};

}  // namespace Inference

#endif
