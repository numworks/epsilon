#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <apps/shared/explicit_float_parameter_controller.h>
#include <apps/shared/with_record.h>
#include <escher/buffer_text_view.h>
#include <escher/chevron_view.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "banner_view.h"
#include "calculation_parameter_controller.h"

namespace Graph {

class GraphController;

class CurveParameterController
    : public Shared::ExplicitFloatParameterController,
      public Shared::WithRecord {
 public:
  CurveParameterController(Shared::InteractiveCurveViewRange* graphRange,
                           BannerView* bannerView,
                           Shared::CurveViewCursor* cursor,
                           GraphView* graphView,
                           GraphController* graphController);
  const char* title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfRows; }
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastTitle;
  }

 private:
  KDCoordinate separatorBeforeRow(int row) override {
    return cell(row) == &m_calculationCell ? k_defaultRowSeparator : 0;
  }

  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override {
    return confirmParameterAtIndex(parameterIndex, f);
  }
  void setRecord(Ion::Storage::Record record) override;
  Escher::HighlightCell* cell(int row) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  Escher::TextField* textFieldOfCellAtRow(int row) override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  bool shouldDisplayCalculation() const;
  bool shouldDisplayFirstDerivative() const;
  void fillParameterCellAtRow(int row) override;
  void updateNumberOfParameterCells();
  /* max(Function::k_maxNameWithArgumentSize + CalculateOnFx,
   * CalculateOnTheCurve + max(Color*Curve)) */
  static constexpr size_t k_titleSize =
      40;  // "Berechnen auf der türkisen Kurve"
  static constexpr int k_indexOfAbscissaCell = 0;
  static constexpr int k_indexOfImageCell1 = 1;
  static constexpr int k_indexOfImageCell2 = 2;
  static constexpr int k_indexOfFirstDerivativeCell1 = 3;
  static constexpr int k_indexOfFirstDerivativeCell2 = 4;
  static constexpr int k_numberOfParameterRows = 5;
  constexpr static int k_numberOfRows = k_numberOfParameterRows + 2;

  using ParameterCell = Escher::MenuCellWithEditableText<
      Escher::OneLineBufferTextView<KDFont::Size::Large>>;

  char m_title[k_titleSize];
  ParameterCell m_parameterCells[k_numberOfParameterRows];
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::ChevronView>
      m_calculationCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::ChevronView>
      m_optionsCell;
  Shared::InteractiveCurveViewRange* m_graphRange;
  Shared::CurveViewCursor* m_cursor;
  PreimageGraphController m_preimageGraphController;
  CalculationParameterController m_calculationParameterController;

  // parent controller: handles the cursor
  GraphController* m_graphController;
};

}  // namespace Graph

#endif
