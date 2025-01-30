#ifndef DISTRIBUTIONS_PROBABILITY_CALCULATION_CONTROLLER_H
#define DISTRIBUTIONS_PROBABILITY_CALCULATION_CONTROLLER_H

#include <apps/shared/parameter_text_field_delegate.h>
#include <escher/buffer_text_view.h>
#include <escher/dropdown_view.h>
#include <escher/regular_table_view_data_source.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "distributions/constants.h"
#include "distributions/models/calculation/calculation.h"
#include "distributions/models/distribution/distribution.h"
#include "distributions/probability/calculation_cell.h"
#include "distributions/probability/calculation_popup_data_source.h"
#include "distributions/probability/distribution_curve_view.h"

namespace Distributions {

class CalculationController : public Escher::ViewController,
                              public Escher::RegularHeightTableViewDataSource,
                              public Escher::SelectableTableViewDataSource,
                              public Shared::ParameterTextFieldDelegate,
                              public Escher::DropdownCallback {
 public:
  CalculationController(Escher::StackViewController* parentResponder,
                        Distribution* distribution, Calculation* calculation);

  void reinitCalculation();

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;

  /* ViewController */
  Escher::View* view() override { return &m_contentView; }
  const char* title() const override { return m_titleBuffer; }
  TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  void viewWillAppear() override;
  void viewDidDisappear() override;

  /* TableViewDataSource */
  int numberOfRows() const override { return 1; }
  int numberOfColumns() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override { return 1; }
  int typeAtLocation(int column, int row) const override { return column; }

  /* TextField delegate */
  void textFieldDidHandleEvent(Escher::AbstractTextField* textField) override;
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  void reload();

  // Escher::Dropdown
  void onDropdownSelected(int selectedRow) override;
  bool popupDidReceiveEvent(Ion::Events::Event event,
                            Escher::Responder* responder) override;

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  constexpr static int k_numberOfCalculationCells = 3;
  constexpr static KDMargins k_tableMargins = KDMargins(3);
  constexpr static const char* k_unknownParameterBannerText = "%s=%*.*ed";
  constexpr static const char* k_parameterTitle = "%s = %*.*ed ";

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int column) override;
  KDCoordinate defaultRowHeight() override;

  void updateTitle();
  void setCalculationAccordingToIndex(int index,
                                      bool forceReinitialisation = false);
  void updateCells();
  void updateCellsValues();

  class ContentView : public Escher::View {
   public:
    ContentView(Escher::SelectableTableView* selectableTableView,
                Distribution* distribution, Calculation* calculation);
    DistributionCurveView* distributionCurveView() {
      return &m_distributionCurveView;
    }
    Escher::AbstractBufferTextView* unknownParameterValue() {
      return &m_unknownParameterBanner;
    }
    void reload() { layoutSubviews(true); }

   private:
    constexpr static KDCoordinate k_bannerHeight =
        Escher::Metric::DisplayHeightWithoutTitleBar / 6;
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 3; };
    Escher::View* subviewAtIndex(int index) override;
    Escher::SelectableTableView* m_selectableTableView;
    DistributionCurveView m_distributionCurveView;
    Escher::OneLineBufferTextView<KDFont::Size::Large> m_unknownParameterBanner;
  };
  Calculation* m_calculation;
  Distribution* m_distribution;
  ContentView m_contentView;
  Escher::SelectableTableView m_selectableTableView;
  CalculationPopupDataSource m_imagesDataSource;
  Escher::Dropdown m_dropdown;
  CalculationCell m_calculationCells[k_numberOfCalculationCells];
  // Max is reached for hypergeometric distribution
  constexpr static int k_titleBufferSize =
      sizeof("N =  K =  n = ") + 3 * Constants::k_shortFloatNumberOfChars;
  char m_titleBuffer[k_titleBufferSize];
};

}  // namespace Distributions

#endif
