#pragma once

#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/layout_view.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <escher/menu_cell.h>
#include <escher/stack_view_controller.h>

#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/models/inference_model.h"
#include "inference_controller.h"

namespace Inference {

namespace ResultsControllerDimensions {
constexpr static int k_numberOfReusableCells = 8;
}

using ResultCell = Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                                    Escher::FloatBufferTextView<>>;

class ResultsController : public InferenceController,
                          public Escher::ListWithTopAndBottomController,
                          public DynamicCellsDataSource<ResultCell> {
 public:
  ResultsController(Escher::Responder* parent, InferenceModel* inference,
                    ControllerContainer* controllerContainer,
                    bool enableHeadline = true);

  static bool ButtonAction(ResultsController* controller, void* s);

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() const override;
  const char* title() const override;
  void initView() override;

  // StandardMemoizedListViewDataSource
  int numberOfRows() const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtRow(int row) const override;
  KDCoordinate separatorBeforeRow(int row) const override {
    return row == m_inferenceModel->secondResultSectionStart() ||
                   typeAtRow(row) == k_buttonCellType
               ? k_defaultRowSeparator
               : 0;
  }
  KDCoordinate nonMemoizedRowHeight(int row) override;

  // DynamicCellsDataSource
  Escher::SelectableListView* tableView() override {
    return &m_selectableListView;
  }

  int numberOfCells() override {
    return ResultsControllerDimensions::k_numberOfReusableCells;
  }

 protected:
  Escher::MessageTextView m_title;
  ControllerContainer* m_controllerContainer;

  constexpr static int k_titleBufferSize = 50;
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];

 private:
  constexpr static int k_numberOfReusableCells = 6;
  constexpr static int k_resultCellType = 0;
  constexpr static int k_buttonCellType = 1;
  Escher::ButtonCell m_next;
};

}  // namespace Inference
