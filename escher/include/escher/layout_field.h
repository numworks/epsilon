#ifndef ESCHER_LAYOUT_FIELD_H
#define ESCHER_LAYOUT_FIELD_H

#include <escher/expression_view.h>
#include <escher/layout_field_delegate.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <escher/text_field.h>
#include <kandinsky/point.h>
#include <poincare/layout_reference.h>
#include <poincare/layout_cursor.h>

class LayoutField : public ScrollableView, public ScrollViewDataSource {
public:
  LayoutField(Responder * parentResponder, Poincare::LayoutRef layoutR, LayoutFieldDelegate * delegate = nullptr) :
    ScrollableView(parentResponder, &m_contentView, this),
    m_contentView(layoutR),
    m_delegate(delegate)
  {}
  void setDelegate(LayoutFieldDelegate * delegate) { m_delegate = delegate; }
  bool isEditing() const { return m_contentView.isEditing(); }
  void setEditing(bool isEditing) { m_contentView.setEditing(isEditing); }
  void clearLayout() { m_contentView.clearLayout(); }
  void scrollToCursor() {
    scrollToBaselinedRect(m_contentView.cursorRect(), m_contentView.cursor()->baseline());
  }
  void reload();
  bool hasText() const { return layoutRef().hasText(); }
  int writeTextInBuffer(char * buffer, int bufferLength) { return layoutRef().writeTextInBuffer(buffer, bufferLength); }
  Poincare::LayoutRef layoutRef() const { return m_contentView.expressionView()->layoutRef(); }
  char XNTChar() { return m_contentView.cursor()->layoutReference().XNTChar(); }

  // ScrollableView
  void setBackgroundColor(KDColor c) override  {
    ScrollableView::setBackgroundColor(c);
    m_contentView.setBackgroundColor(c);
  }

  /* Responder */
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  Toolbox * toolbox() override {
    return m_delegate != nullptr ? m_delegate->toolboxForLayoutField(this) : nullptr;
  }
  bool layoutFieldShouldFinishEditing(Ion::Events::Event event) { // TODO REMOVE ?
    return m_delegate->layoutFieldShouldFinishEditing(this, event);
  }

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override {
    KDSize contentViewSize = m_contentView.minimalSizeForOptimalDisplay();
    return KDSize(contentViewSize.width(), contentViewSize.height());
  }

protected:
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);

private:
  constexpr static int k_maxNumberOfLayouts = 152;
  static_assert(k_maxNumberOfLayouts == TextField::maxBufferSize(), "Maximal number of layouts in a layout field should be equal to max number of char in text field");
  void scrollRightOfLayout(Poincare::LayoutRef layoutR);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);
  void insertLayoutAtCursor(Poincare::LayoutRef layoutR, Poincare::LayoutRef pointedLayoutRef, bool forceCursorRightOfLayout = false);

  class ContentView : public View {
  public:
    ContentView(Poincare::LayoutRef layoutR) :
      m_cursor(layoutR, Poincare::LayoutCursor::Position::Right),
      m_expressionView(0.0f, 0.5f, KDColorBlack, KDColorWhite),
      m_cursorView(),
      m_isEditing(false)
    {
      m_expressionView.setLayoutRef(layoutR);
    }

    bool isEditing() const { return m_isEditing; }
    void setEditing(bool isEditing) {
      m_isEditing = isEditing;
      markRectAsDirty(bounds());
      layoutSubviews();
    }

    void setBackgroundColor(KDColor c) { m_expressionView.setBackgroundColor(c); }
    void setCursor(Poincare::LayoutCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged() { layoutCursorSubview(); }
    KDRect cursorRect() { return m_cursorView.frame(); }
    Poincare::LayoutCursor * cursor() { return &m_cursor; }
    const ExpressionView * expressionView() const { return &m_expressionView; }
    ExpressionView * editableExpressionView() { return &m_expressionView; }
    void clearLayout() { m_cursor.clearLayout(); }
    /* View */
    KDSize minimalSizeForOptimalDisplay() const override {
      KDSize evSize = m_expressionView.minimalSizeForOptimalDisplay();
      return KDSize(evSize.width() + Poincare::LayoutCursor::k_cursorWidth, evSize.height());
    }
  private:
    enum class Position {
      Top,
      Bottom
    };
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override {
      assert(index >= 0 && index < 2);
      View * m_views[] = {&m_expressionView, &m_cursorView};
      return m_views[index];
    }
    void layoutSubviews() override {
      m_expressionView.setFrame(bounds());
      layoutCursorSubview();
    }
    void layoutCursorSubview();
    Poincare::LayoutCursor m_cursor;
    ExpressionView m_expressionView;
    TextCursorView m_cursorView;
    bool m_isEditing;
  };
  ContentView m_contentView;
  LayoutFieldDelegate * m_delegate;
};

#endif
