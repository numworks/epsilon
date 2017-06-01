#ifndef SEQUENCE_TERM_SUM_CONTROLLER_H
#define SEQUENCE_TERM_SUM_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "curve_view_range.h"
#include "vertical_cursor_view.h"
#include "../../shared/curve_view_cursor.h"

namespace Sequence {

class TermSumController : public ViewController {
public:
  TermSumController(Responder * parentResponder, GraphView * graphView, CurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  View * view() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool moveCursorHorizontallyToPosition(int position);
  void setSequence(Sequence * sequence);
private:
  constexpr static float k_cursorTopMarginRatio = 0.07f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorRightMarginRatio = 0.04f; // (cursorWidth/2)/graphViewWidth
  constexpr static float k_cursorBottomMarginRatio = 0.28f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  constexpr static float k_cursorLeftMarginRatio = 0.04f;  // (cursorWidth/2)/graphViewWidth
  class ContentView : public View {
  public:
    class LegendView : public View {
    public:
      LegendView();
      ~LegendView();
      LegendView(const LegendView& other) = delete;
      LegendView(LegendView&& other) = delete;
      LegendView& operator=(const LegendView& other) = delete;
      LegendView& operator=(LegendView&& other) = delete;
      void drawRect(KDContext * ctx, KDRect rect) const override;
      void setLegendMessage(I18n::Message message);
      void setSumSubscript(float start);
      void setSumSuperscript(float start, float end);
      void setSumResult(const char * sequenceName, float result);
    private:
      void layoutSubviews() override;
      int numberOfSubviews() const override;
      View * subviewAtIndex(int index) override;
      ExpressionView m_sum;
      Poincare::ExpressionLayout * m_sumLayout;
      MessageTextView m_legend;
    };
    ContentView(GraphView * graphView);
    void layoutSubviews() override;
    GraphView * graphView();
    LegendView * legendView();
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    GraphView * m_graphView;
    LegendView m_legendView;
    constexpr static KDCoordinate k_legendHeight = 35;
  };
  ContentView m_contentView;
  CurveViewRange * m_graphRange;
  Sequence * m_sequence;
  Shared::CurveViewCursor * m_cursor;
  VerticalCursorView m_cursorView;
  /* The user can move the cursor to an abscissa n by typing the right digits.
   * To be able to go to abscissa represented by more than one digit, we record
   * the value typed by the used up to now (if he typed '1' and '4',
   * m_bufferCursorPosition = 14). */
  int m_bufferCursorPosition;
  int m_step;
  int m_startSum;
  int m_endSum;
};

}

#endif
