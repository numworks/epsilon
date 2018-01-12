#ifndef SHARED_SUM_GRAPH_CONTROLLER_H
#define SHARED_SUM_GRAPH_CONTROLLER_H

#include <escher.h>
#include "function_graph_view.h"
#include "interactive_curve_view_range.h"
#include "vertical_cursor_view.h"
#include "curve_view_cursor.h"
#include "simple_interactive_curve_view_controller.h"
#include "function.h"
#include "text_field_delegate.h"

namespace Shared {

class SumGraphController : public SimpleInteractiveCurveViewController, public TextFieldDelegate {
public:
  SumGraphController(Responder * parentResponder, FunctionGraphView * curveView, InteractiveCurveViewRange * range, CurveViewCursor * cursor, char sumSymbol);
  void viewWillAppear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Function * function);
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField, const char * text) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
protected:
  virtual bool moveCursorHorizontallyToPosition(double position);
  enum class Step {
    FirstParameter = 0,
    SecondParameter = 1,
    Result = 2
  };
  Step m_step;
  double m_startSum;
  double m_endSum;
  Function * m_function;
  InteractiveCurveViewRange * m_graphRange;
private:
  constexpr static float k_cursorTopMarginRatio = 0.06f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.28f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  virtual I18n::Message legendMessageAtStep(Step step) = 0;
  virtual double cursorNextStep(double position, int direction) = 0;
  virtual Poincare::ExpressionLayout * createFunctionLayout(const char * functionName) = 0;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  Shared::CurveView * curveView() override { return m_graphView; }
  TextFieldDelegateApp * textFieldDelegateApp() override {
    return static_cast<TextFieldDelegateApp *>(app());
  }
  bool handleEnter() override;
  class LegendView : public View {
  public:
    LegendView(SumGraphController * controller, char sumSymbol);
    ~LegendView();
    LegendView(const LegendView& other) = delete;
    LegendView(LegendView&& other) = delete;
    LegendView& operator=(const LegendView& other) = delete;
    LegendView& operator=(LegendView&& other) = delete;
    TextField * textField() { return &m_editableZone; }
    KDSize minimalSizeForOptimalDisplay() const override;
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setLegendMessage(I18n::Message message, Step step);
    void setEditableZone(double d);
    void setSumSymbol(Step step, double start = NAN, double end = NAN, double result = NAN, Poincare::ExpressionLayout * sequenceName = nullptr);
  private:
    constexpr static KDCoordinate k_legendHeight = 35;
    constexpr static KDCoordinate k_editableZoneWidth = 12*KDText::charSize(KDText::FontSize::Small).width();
    constexpr static KDCoordinate k_editableZoneHeight = KDText::charSize(KDText::FontSize::Small).height();
    constexpr static KDCoordinate k_symbolHeightMargin = 8;
    constexpr static KDCoordinate k_sigmaHeight = 18;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void layoutSubviews(Step step);
    ExpressionView m_sum;
    Poincare::ExpressionLayout * m_sumLayout;
    MessageTextView m_legend;
    TextField m_editableZone;
    char m_draftText[TextField::maxBufferSize()];
    char m_sumSymbol;
  };
  FunctionGraphView * m_graphView;
  LegendView m_legendView;
  VerticalCursorView m_cursorView;
};

}

#endif
