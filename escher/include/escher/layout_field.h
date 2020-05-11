#ifndef ESCHER_LAYOUT_FIELD_H
#define ESCHER_LAYOUT_FIELD_H

#include <escher/editable_field.h>
#include <escher/expression_view.h>
#include <escher/layout_field_delegate.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <escher/text_field.h>
#include <kandinsky/point.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

// See TODO in EditableField

class LayoutField : public ScrollableView, public ScrollViewDataSource, public EditableField {
public:
  LayoutField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, LayoutFieldDelegate * delegate = nullptr) :
    ScrollableView(parentResponder, &m_contentView, this),
    EditableField(inputEventHandlerDelegate),
    m_contentView(),
    m_delegate(delegate)
  {}
  void setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate, LayoutFieldDelegate * delegate) { m_inputEventHandlerDelegate = inputEventHandlerDelegate; m_delegate = delegate; }
  Poincare::Context * context() const;
  bool isEditing() const override { return m_contentView.isEditing(); }
  void setEditing(bool isEditing) override;
  void clearLayout();
  void scrollToCursor() {
    scrollToBaselinedRect(m_contentView.cursorRect(), m_contentView.cursor()->baselineWithoutSelection());
  }
  bool hasText() const { return layout().hasText(); }
  Poincare::Layout layout() const { return m_contentView.expressionView()->layout(); }
  CodePoint XNTCodePoint(CodePoint defaultXNTCodePoint) override;
  void putCursorRightOfLayout();

  // ScrollableView
  void setBackgroundColor(KDColor c) override  {
    ScrollableView::setBackgroundColor(c);
    m_contentView.setBackgroundColor(c);
  }

  /* Responder */
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  // TODO: factorize with TextField (see TODO of EditableField)
  bool shouldFinishEditing(Ion::Events::Event event) override;

  // Selection
  bool resetSelection() { return m_contentView.resetSelection(); }
  void deleteSelection();

private:
  constexpr static int k_maxNumberOfLayouts = 220;
  static_assert(k_maxNumberOfLayouts == TextField::maxBufferSize(), "Maximal number of layouts in a layout field should be equal to max number of char in text field");
  void reload(KDSize previousSize);
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  bool privateHandleSelectionEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  void scrollRightOfLayout(Poincare::Layout layoutR);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);
  void insertLayoutAtCursor(Poincare::Layout layoutR, Poincare::Expression correspondingExpression, bool forceCursorRightOfLayout = false);

  class ContentView : public View {
  public:
    ContentView();
    bool isEditing() const { return m_isEditing; }
    bool setEditing(bool isEditing); // returns True if LayoutField should reload
    void setBackgroundColor(KDColor c) { m_expressionView.setBackgroundColor(c); }
    void setCursor(Poincare::LayoutCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged() { layoutCursorSubview(false); }
    KDRect cursorRect() { return m_cursorView.frame(); }
    Poincare::LayoutCursor * cursor() { return &m_cursor; }
    const ExpressionView * expressionView() const { return &m_expressionView; }
    void clearLayout();
    // View
    KDSize minimalSizeForOptimalDisplay() const override;
    // Selection
    Poincare::Layout * selectionStart() { return &m_selectionStart; }
    Poincare::Layout * selectionEnd() { return &m_selectionEnd; }
    void addSelection(Poincare::Layout addedLayout);
    bool resetSelection(); // returns true if the selection was indeed reset
    void copySelection(Poincare::Context * context);
    bool selectionIsEmpty() const;
    void deleteSelection();

  private:
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void layoutCursorSubview(bool force);
    KDRect selectionRect() const;
    Poincare::LayoutCursor m_cursor;
    ExpressionView m_expressionView;
    TextCursorView m_cursorView;
    /* The selection starts on the left of m_selectionStart, and ends on the
     * right of m_selectionEnd. */
    Poincare::Layout m_selectionStart;
    Poincare::Layout m_selectionEnd;
    bool m_isEditing;
  };
  ContentView m_contentView;
  LayoutFieldDelegate * m_delegate;
};

#endif
