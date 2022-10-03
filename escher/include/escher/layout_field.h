#ifndef ESCHER_LAYOUT_FIELD_H
#define ESCHER_LAYOUT_FIELD_H

#include <escher/clipboard.h>
#include <escher/editable_field.h>
#include <escher/expression_view.h>
#include <escher/layout_field_delegate.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <escher/text_field.h>
#include <kandinsky/point.h>
#include <poincare/layout_cursor.h>

// See TODO in EditableField

namespace Escher {

class LayoutField : public ScrollableView, public ScrollViewDataSource, public EditableField {
public:
  LayoutField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, LayoutFieldDelegate * delegate = nullptr, KDFont::Size font = KDFont::Size::Large) :
    ScrollableView(parentResponder, &m_contentView, this),
    EditableField(inputEventHandlerDelegate),
    m_contentView(font),
    m_insertionCursorEvent(Ion::Events::None),
    m_delegate(delegate)
  {}
  void setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate, LayoutFieldDelegate * delegate) { m_inputEventHandlerDelegate = inputEventHandlerDelegate; m_delegate = delegate; }
  Poincare::Context * context() const;
  bool isEditing() const override { return m_contentView.isEditing(); }
  void setEditing(bool isEditing) override;
  void clearLayout();
  void scrollToCursor() {
    scrollToBaselinedRect(m_contentView.cursorRect(), m_contentView.cursor()->baselineWithoutSelection(m_contentView.font()));
  }
  bool hasText() const { return layout().hasText(); }
  Poincare::Layout layout() const { return m_contentView.expressionView()->layout(); }
  bool layoutHasNode() const { return m_contentView.expressionView()->layoutHasNode(); }
  bool addXNTCodePoint(CodePoint defaultXNTCodePoint) override;
  void putCursorRightOfLayout();
  void putCursorLeftOfLayout();
  void setInsertionCursorEvent(Ion::Events::Event event) { m_insertionCursorEvent = event; }
  void setLayout(Poincare::Layout newLayout);
  void tidy();

  // ScrollableView
  void setBackgroundColor(KDColor c) override  {
    ScrollableView::setBackgroundColor(c);
    m_contentView.setBackgroundColor(c);
  }
  /* Always keep the full margins on a layout field, as it would otherwise lead
   * to weird cropping during edition. */
  float marginPortionTolerance() const override { return 0.f; }

  /* Responder */
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  // TODO: factorize with TextField (see TODO of EditableField)
  bool shouldFinishEditing(Ion::Events::Event event) override;

  // Selection
  bool resetSelection() { return m_contentView.resetSelection(); }
  void deleteSelection();

  Poincare::LayoutCursor * cursor() { return m_contentView.cursor(); }

private:
  constexpr static int k_maxNumberOfLayouts = 220;
  static_assert(k_maxNumberOfLayouts == TextField::maxBufferSize(), "Maximal number of layouts in a layout field should be equal to max number of char in text field");
  void reload(KDSize previousSize);
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  bool privateHandleSelectionEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  void scrollRightOfLayout(Poincare::Layout layoutR);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);
  void insertLayoutAtCursor(Poincare::Layout layoutR, Poincare::Expression correspondingExpression, bool forceCursorRightOfLayout = false, bool forceCursorLeftOfText = false);
  bool eventShouldUpdateInsertionCursor(Ion::Events::Event event) { return event == m_insertionCursorEvent; }

  class ContentView : public View {
  public:
    ContentView(KDFont::Size font);
    bool isEditing() const { return m_isEditing; }
    bool setEditing(bool isEditing); // returns True if LayoutField should reload
    void setBackgroundColor(KDColor c) { m_expressionView.setBackgroundColor(c); }
    void setCursor(Poincare::LayoutCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged() { layoutCursorSubview(false); }
    KDRect cursorRect() { return m_cursorView.frame(); }
    Poincare::LayoutCursor * cursor() { return &m_cursor; }
    const ExpressionView * expressionView() const { return &m_expressionView; }
    void clearLayout();
    void tidy();
    // View
    KDSize minimalSizeForOptimalDisplay() const override;
    // Selection
    Poincare::Layout * selectionStart() { return &m_selectionStart; }
    Poincare::Layout * selectionEnd() { return &m_selectionEnd; }
    void addSelection(Poincare::Layout addedLayout);
    bool resetSelection(); // returns true if the selection was indeed reset
    void copySelection(Poincare::Context * context, Escher::Clipboard * clipboard);
    bool selectionIsEmpty() const;
    void deleteSelection();
    void invalidateInsertionCursor() { m_insertionCursor = Poincare::LayoutCursor(); }
    void updateInsertionCursor();
    KDFont::Size font() const { return m_expressionView.font(); }

  private:
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void layoutCursorSubview(bool force);
    void useInsertionCursor();
    KDRect selectionRect() const;
    Poincare::LayoutCursor m_cursor;
    /* The insertion cursor is a secondary cursor that determines where text
     * should be inserted. Most of the time this cursor is useless (and is
     * therefore disabled), but in an interface where the user can navigate out
     * of the field, it's important to keep track of where inserted text should
     * go even if the main cursor was moved.
     * For instance, this is useful in the Calculation app when the user wants
     * to type a division and scroll up the history to insert something at the
     * denominator. */
    Poincare::LayoutCursor m_insertionCursor;
    ExpressionView m_expressionView;
    TextCursorView m_cursorView;
    /* The selection starts on the left of m_selectionStart, and ends on the
     * right of m_selectionEnd. */
    Poincare::Layout m_selectionStart;
    Poincare::Layout m_selectionEnd;
    bool m_isEditing;
  };
  ContentView m_contentView;
  Ion::Events::Event m_insertionCursorEvent;
  LayoutFieldDelegate * m_delegate;
};

}

#endif
