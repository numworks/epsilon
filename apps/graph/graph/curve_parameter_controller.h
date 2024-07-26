#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <apps/shared/explicit_float_parameter_controller.h>
#include <escher/buffer_text_view.h>
#include <escher/chevron_view.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "../shared/derivative_column_parameter_controller.h"
#include "banner_view.h"
#include "calculation_parameter_controller.h"

namespace Graph {

class GraphController;
class FunctionParameterController;

class CurveParameterController
    : public Shared::ExplicitFloatParameterController,
      public ParameterDelegate {
 public:
  CurveParameterController(
      Shared::InteractiveCurveViewRange* graphRange, BannerView* bannerView,
      Shared::CurveViewCursor* cursor, GraphView* graphView,
      GraphController* graphController,
      FunctionParameterController* functionParameterController,
      DerivativeColumnParameterController* derivativeColumnParameterController);
  const char* title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfRows; }
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastTitle;
  }

  // ParameterDelegate
  bool usePersonalizedTitle() const override { return false; }
  bool showColorCell() const override { return true; }
  I18n::Message hideDerivativeMessage() const override {
    return I18n::Message::HideDerivative;
  }
  void hideDerivative(Ion::Storage::Record record,
                      int derivationOrder) override;

  void setRecord(Ion::Storage::Record record, int derivationOrder);

 private:
  using ParameterCell = Escher::MenuCellWithEditableText<
      Escher::OneLineBufferTextView<KDFont::Size::Large>>;
  enum class ParameterIndex {
    Abscissa = 0,
    Image1,
    Image2,
    Image3,
    FirstDerivative1,
    FirstDerivative2,
    SecondDerivative1,
    SecondDerivative2,
    NumberOfParameters,
  };

  KDCoordinate separatorBeforeRow(int row) override {
    return cell(row) == &m_calculationCell ? k_defaultRowSeparator : 0;
  }
  ParameterCell* parameterCell(ParameterIndex index) {
    return &m_parameterCells[static_cast<int>(index)];
  }
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override {
    return confirmParameterAtIndex(parameterIndex, f);
  }
  Escher::HighlightCell* cell(int row) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  Escher::TextField* textFieldOfCellAtRow(int row) override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  void fillParameterCellAtRow(int row) override;
  bool parameterAtRowIsFirstComponent(int row) const;
  int derivationOrderOfParameterAtRow(int row) const;
  Escher::StackViewController* stackController() const;

  /* max(Function::k_maxNameWithArgumentSize + CalculateOnFx,
   * CalculateOnTheCurve + max(Color*Curve)) */
  static constexpr size_t k_titleSize =
      40;  // "Berechnen auf der türkisen Kurve"
  constexpr static int k_numberOfParameterRows =
      static_cast<int>(ParameterIndex::NumberOfParameters);
  constexpr static int k_numberOfRows = k_numberOfParameterRows + 2;

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
  FunctionParameterController* m_functionParameterController;
  DerivativeColumnParameterController* m_derivativeColumnParameterController;
  Ion::Storage::Record m_record;
  int m_derivationOrder;

  // parent controller: handles the cursor
  GraphController* m_graphController;
};

}  // namespace Graph

#endif
