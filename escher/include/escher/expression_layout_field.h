#ifndef ESCHER_EXPRESSION_LAYOUT_FIELD_H
#define ESCHER_EXPRESSION_LAYOUT_FIELD_H

#include <escher/expression_view.h>
#include <escher/expression_layout_field_delegate.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <kandinsky/point.h>
#include <poincare/expression_layout.h>
#include <poincare/expression_layout_cursor.h>

class ExpressionLayoutField : public ScrollableView, public ScrollViewDataSource {
public:
  ExpressionLayoutField(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, ExpressionLayoutFieldDelegate * delegate = nullptr);
  void setDelegate(ExpressionLayoutFieldDelegate * delegate) { m_delegate = delegate; }
  bool isEditing() const;
  void setEditing(bool isEditing);
  void clearLayout();
  void scrollToCursor();
  void reload();
  bool hasText() const;
  int writeTextInBuffer(char * buffer, int bufferLength);
  bool handleEventWithText(const char * text, bool indentation = false) override;
  Poincare::ExpressionLayout * expressionLayout();
  char XNTChar();
  void setBackgroundColor(KDColor c) override;

  /* Responder */
  Toolbox * toolbox() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool expressionLayoutFieldShouldFinishEditing(Ion::Events::Event event);

  void insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout);

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

protected:
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
private:
  void scrollRightOfLayout(Poincare::ExpressionLayout * layout);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);

  class ContentView : public View {
  public:
    ContentView(Poincare::ExpressionLayout * expressionLayout);
    bool isEditing() const { return m_isEditing; }
    void setEditing(bool isEditing);
    void setBackgroundColor(KDColor c);
    void setCursor(Poincare::ExpressionLayoutCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged();
    KDRect cursorRect();
    Poincare::ExpressionLayoutCursor * cursor() { return &m_cursor; }
    const ExpressionView * expressionView() const { return &m_expressionView; }
    ExpressionView * editableExpressionView() { return &m_expressionView; }
    void clearLayout();
    /* View */
    KDSize minimalSizeForOptimalDisplay() const override;
  private:
    enum class Position {
      Top,
      Bottom
    };
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void layoutCursorSubview();
    Poincare::ExpressionLayoutCursor m_cursor;
    ExpressionView m_expressionView;
    TextCursorView m_cursorView;
    bool m_isEditing;
  };
  ContentView m_contentView;
  ExpressionLayoutFieldDelegate * m_delegate;
};

#endif
