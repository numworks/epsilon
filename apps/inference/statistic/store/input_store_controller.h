#ifndef INFERENCE_STATISTIC_INPUT_STORE_CONTROLLER_H
#define INFERENCE_STATISTIC_INPUT_STORE_CONTROLLER_H

#include <escher/buffer_text_highlight_cell.h>

#include "../categorical_controller.h"
#include "../input_controller.h"
#include "store_column_parameter_controller.h"
#include "store_table_cell.h"

namespace Inference {

class InputStoreController : public InputCategoricalController,
                             Escher::DropdownCallback {
 public:
  InputStoreController(Escher::StackViewController* parent,
                       Escher::ViewController* nextController,
                       PageIndex pageIndex,
                       InputStoreController* nextInputStoreController,
                       Statistic* statistic, Poincare::Context* context);

  static bool ButtonAction(InputStoreController* controller, void* s);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char* title() const override {
    InputController::InputTitle(this, m_statistic, m_titleBuffer,
                                InputController::k_titleBufferSize);
    return m_titleBuffer;
  }
  ViewController::TitlesDisplay titlesDisplay() const override {
    if (m_statistic->subApp() == Statistic::SubApp::Interval) {
      return ViewController::TitlesDisplay::DisplayLastTitle;
    }
    if (!m_statistic->canChooseDataset()) {
      return ViewController::TitlesDisplay::DisplayLastTwoTitles;
    }
    if (m_pageIndex == PageIndex::One) {
      return ViewController::TitlesDisplay::DisplayLastAndThirdToLast;
    }
    return ViewController::TitlesDisplay::SameAsPreviousPage;
  }
  void viewWillAppear() override;
  void initView() override;

  // DropdownCallback
  void onDropdownSelected(int selectedRow) override;

  // CategoricalController
  KDCoordinate separatorBeforeRow(int row) const override;

  // ListViewDataSource
  bool canSelectCellAtRow(int row) override {
    return explicitCellAtRow(row)->isVisible();
  }

  void initSeriesSelection() {
    selectSeriesForDropdownRow(m_dropdownCell.dropdown()->selectedRow());
  }

 private:
  class DropdownDataSource : public Escher::ExplicitListViewDataSource {
   public:
    int numberOfRows() const override { return k_numberOfRows; }
    Escher::HighlightCell* cell(int row) override {
      assert(0 <= row && row < k_numberOfRows);
      return &m_cells[row];
    }

   private:
    constexpr static int k_numberOfRows =
        Shared::DoublePairStore::k_numberOfSeries;
    Escher::SmallBufferTextHighlightCell m_cells[k_numberOfRows];
  };

  class PrivateStackViewController
      : public Escher::StackViewController::Default {
   public:
    using Escher::StackViewController::Default::Default;
    TitlesDisplay titlesDisplay() const override { return m_titlesDisplay; }
    void setTitlesDisplay(TitlesDisplay titlesDisplay) {
      m_titlesDisplay = titlesDisplay;
    }

   private:
    TitlesDisplay m_titlesDisplay;
  };

  constexpr static int k_dropdownCellIndex = 0;
  constexpr static int k_maxNumberOfExtraParameters = 2;

  const Escher::HighlightCell* explicitCellAtRow(int row) const override;
  Escher::HighlightCell* explicitCellAtRow(int row) override;
  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_storeTableCell;
  }
  void createDynamicCells() override;
  int indexOfTableCell() const override { return k_dropdownCellIndex + 1; }
  int indexOfSignificanceCell() const override {
    return indexOfFirstExtraParameter() + numberOfExtraParameters();
  }
  int indexOfFirstExtraParameter() const { return indexOfTableCell() + 1; }
  Escher::StackViewController* stackController() const {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }
  const Shared::DoublePairStore* store() const {
    return m_storeTableCell.store();
  }
  int numberOfExtraParameters() const {
    return m_statistic->distributionType() == DistributionType::Z
               ? m_statistic->significanceTestType() ==
                         SignificanceTestType::TwoMeans
                     ? 2
                     : 1
               : 0;
  }
  int indexOfEditedParameterAtIndex(int index) const override;

  void initializeDropdown();
  void selectSeriesForDropdownRow(int row);

  void updateParameterCellsVisibility();
  void hideOtherPageParameterCells();
  void setAllParameterCellsVisible();

  bool shouldDisplayTwoPages() const {
    return m_statistic->significanceTestType() ==
           SignificanceTestType::TwoMeans;
  }

  bool areAllParameterCellsInvisible() const;

  DropdownDataSource m_dropdownDataSource;
  DropdownCategoricalCell m_dropdownCell;
  InputCategoricalCell<Escher::LayoutView>
      m_extraParameters[k_maxNumberOfExtraParameters];
  StoreTableCell m_storeTableCell;
  /* This second stack view controller is used to make the banner of the store
   * parameter controller white, which deviates from the style of the main
   * stack view controller (gray scales). */
  PrivateStackViewController m_secondStackController;
  StoreColumnParameterController m_storeParameterController;
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[InputController::k_titleBufferSize];
  Statistic::SubApp m_loadedSubApp;
  DistributionType m_loadedDistribution;
  SignificanceTestType m_loadedTest;

  /* There can be several instances of InputStoreController, each representing a
   * distinct dataset selection page. This is used only for some test categories
   * (e.g. TwoMeansTest). */
  PageIndex m_pageIndex;
  InputStoreController* m_nextInputStoreController;
  ViewController* m_nextOtherController;
};

}  // namespace Inference

#endif
