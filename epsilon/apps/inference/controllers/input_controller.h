#ifndef INFERENCE_STATISTIC_INPUT_CONTROLLER_H
#define INFERENCE_STATISTIC_INPUT_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/highlight_cell.h>
#include <escher/menu_cell_with_editable_text.h>

#include "inference/controllers/dynamic_cells_data_source.h"
#include "inference/models/inference_model.h"
#include "results_controller.h"

namespace Inference {

using ParameterCell = Escher::MenuCellWithEditableText<Escher::LayoutView,
                                                       Escher::MessageTextView>;

class InputController
    : public Shared::FloatParameterController<double>,
      public DynamicCellsDataSource<ParameterCell, k_maxNumberOfParameterCell>,
      public DynamicCellsDataSourceDelegate<ParameterCell> {
  friend class InputStoreController;

 public:
  constexpr static int k_numberOfTitleSignificantDigits =
      Poincare::Preferences::VeryShortNumberOfSignificantDigits;
  constexpr static int k_titleBufferSize =
      sizeof("H0:μ1-μ2= Ha:μ1-μ2≠ α=") +  // longest possible
      3 * (Poincare::PrintFloat::charSizeForFloatsWithPrecision(
              k_numberOfTitleSignificantDigits));
  constexpr static int k_numberOfReusableCells =
      Ion::Display::Height /
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight +
      2;

  static void InputTitle(const Escher::ViewController* vc,
                         const InferenceModel* inference, char* titleBuffer,
                         size_t titleBufferSize);

  InputController(Escher::StackViewController* parent,
                  ResultsController* resultsController,
                  InferenceModel* inference);
  int numberOfRows() const override {
    return m_inference->numberOfParameters() + 1 /* button */;
  }
  const char* title() const override {
    InputTitle(this, m_inference, m_titleBuffer, k_titleBufferSize);
    return m_titleBuffer;
  }
  ViewController::TitlesDisplay titlesDisplay() const override;
  void initView() override;
  void viewWillAppear() override;
  int typeAtRow(int row) const override;
  bool handleEvent(Ion::Events::Event event) override;
  void buttonAction() override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate separatorBeforeRow(int row) const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;

  void initCell(ParameterCell, void* cell, int index) override;
  Escher::SelectableTableView* tableView() override {
    return &m_selectableListView;
  }

 protected:
  double parameterAtIndex(int i) override {
    return m_inference->parameterAtIndex(i);
  }

 private:
  int reusableParameterCellCount(int type) const override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;

  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];
  InferenceModel* m_inference;
  ResultsController* m_resultsController;

  constexpr static int k_significanceCellType = 2;

  Escher::MenuCellWithEditableText<Escher::MessageTextView,
                                   Escher::MessageTextView>
      m_significanceCell;
  Escher::MessageTextView m_messageView;
};

}  // namespace Inference

#endif
