#ifndef SHARED_SUM_GRAPH_CONTROLLER_H
#define SHARED_SUM_GRAPH_CONTROLLER_H

#include <escher.h>
#include <poincare/print_float.h>
#include "function_graph_view.h"
#include "vertical_cursor_view.h"
#include "simple_interactive_curve_view_controller.h"
#include "function.h"
#include "text_field_delegate.h"
#include "expiring_pointer.h"

namespace Shared {

class SumGraphController : public SimpleInteractiveCurveViewController {
public:
  SumGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, FunctionGraphView * curveView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, CodePoint sumSymbol);
  void viewWillAppear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("Sum");
  void setRecord(Ion::Storage::Record record);
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
protected:
  virtual bool moveCursorHorizontallyToPosition(double position);
  enum class Step {
    FirstParameter = 0,
    SecondParameter = 1,
    Result = 2
  };
  Step m_step;
  double m_startSum;
  Ion::Storage::Record m_record;
  InteractiveCurveViewRange * m_graphRange;
private:
  float cursorTopMarginRatio() override { return 0.06f; }
  float cursorBottomMarginRatio() override { return 0.28f; }
  bool handleLeftRightEvent(Ion::Events::Event event) override;
  bool handleEnter() override;
  void reloadBannerView() override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::CurveView * curveView() override { return m_graphView; }
  virtual I18n::Message legendMessageAtStep(Step step) = 0;
  virtual double cursorNextStep(double position, int direction) = 0;
  virtual Poincare::Layout createFunctionLayout(ExpiringPointer<Function> function) = 0;
  class LegendView : public View {
  public:
    LegendView(SumGraphController * controller, InputEventHandlerDelegate * inputEventHandlerDelegate, CodePoint sumSymbol);
    LegendView(const LegendView& other) = delete;
    LegendView(LegendView&& other) = delete;
    LegendView& operator=(const LegendView& other) = delete;
    LegendView& operator=(LegendView&& other) = delete;
    TextField * textField() { return &m_editableZone; }
    KDSize minimalSizeForOptimalDisplay() const override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setLegendMessage(I18n::Message message, Step step);
    void setEditableZone(double d);
    void setSumLayout(Step step, double start, double end, double result, Poincare::Layout functionLayout);
  private:
    constexpr static size_t k_editableZoneBufferSize = Poincare::PrintFloat::k_maxFloatCharSize;
    constexpr static int k_valuesPrecision = Poincare::Preferences::MediumNumberOfSignificantDigits;
    constexpr static int k_valuesBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(k_valuesPrecision);
    constexpr static KDCoordinate k_legendHeight = 35;
    constexpr static const KDFont * k_font = KDFont::SmallFont;
    static KDCoordinate editableZoneWidth() { return 12*k_font->glyphSize().width(); }
    static KDCoordinate editableZoneHeight() { return k_font->glyphSize().height(); }
    constexpr static KDCoordinate k_symbolHeightMargin = 8;
    constexpr static KDCoordinate k_sigmaHeight = 18;
    int numberOfSubviews() const override { return 3; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void layoutSubviews(Step step, bool force);
    ExpressionView m_sum;
    MessageTextView m_legend;
    TextField m_editableZone;
    char m_textBuffer[k_editableZoneBufferSize];
    CodePoint m_sumSymbol;
  };
  FunctionGraphView * m_graphView;
  LegendView m_legendView;
  VerticalCursorView m_cursorView;
};

}

#endif
