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
                       Escher::ViewController* resultsController,
                       Statistic* statistic, Poincare::Context* context);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char* title() override {
    InputController::InputTitle(this, m_statistic, m_titleBuffer,
                                InputController::k_titleBufferSize);
    return m_titleBuffer;
  }
  ViewController::TitlesDisplay titlesDisplay() override {
    return m_statistic->subApp() == Statistic::SubApp::Interval
               ? ViewController::TitlesDisplay::DisplayLastTitle
               : ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  void viewWillAppear() override;

  // DropdownCallback
  void onDropdownSelected(int selectedRow) override;

 private:
  class DropdownDataSource : public Escher::ExplicitListViewDataSource {
   public:
    int numberOfRows() const override { return k_numberOfRows; }
    Escher::HighlightCell* cell(int row) override { return &m_cells[row]; }

   private:
    constexpr static int k_numberOfRows = 3;  // FIXME
    Escher::SmallBufferTextHighlightCell m_cells[k_numberOfRows];
  };

  class PrivateStackViewController
      : public Escher::StackViewController::Default {
   public:
    using Escher::StackViewController::Default::Default;
    TitlesDisplay titlesDisplay() override { return m_titlesDisplay; }
    void setTitlesDisplay(TitlesDisplay titlesDisplay) {
      m_titlesDisplay = titlesDisplay;
    }

   private:
    TitlesDisplay m_titlesDisplay;
  };

  constexpr static int k_dropdownCellIndex = 0;
  constexpr static int k_maxNumberOfExtraParameters = 1;

  KDCoordinate nonMemoizedRowHeight(int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  InputCategoricalTableCell* categoricalTableCell() override {
    return &m_slopeTableCell;
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
  char listPrefix(int column) const {
    return m_slopeTableCell.store()->columnNamePrefixAtIndex(column);
  }
  int numberOfExtraParameters() const {
    return m_statistic->distributionType() == DistributionType::Z ? 1 : 0;
  }
  int indexOfEditedParameterAtIndex(int index) const override;

  DropdownDataSource m_dropdownDataSource;
  DropdownCategoricalCell m_dropdownCell;
  InputCategoricalCell<Escher::LayoutView>
      m_extraParameters[k_maxNumberOfExtraParameters];
  StoreTableCell m_slopeTableCell;
  /* This second stack view controller is used to make the banner of the store
   * parameter controller white, which deviates from the style of the main
   * stack view controller (gray scales). */
  PrivateStackViewController m_secondStackController;
  StoreColumnParameterController m_storeParameterController;
  char m_titleBuffer[InputController::k_titleBufferSize];
};

}  // namespace Inference

#endif
