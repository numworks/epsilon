#ifndef ESCHER_LAYOUT_FIELD_H
#define ESCHER_LAYOUT_FIELD_H

#include <escher/editable_field.h>
#include <escher/expression_view.h>
#include <escher/layout_field_delegate.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <escher/text_field.h>
#include <kandinsky/point.h>
#include <poincare/layout_cursor.h>
#include <poincare/preferences.h>

// See TODO in EditableField

namespace Escher {

class LayoutField : public WithBlinkingTextCursor<ScrollableView>,
                    public ScrollViewDataSource,
                    public EditableField {
  friend class ExpressionField;

 public:
  LayoutField(Responder* parentResponder,
              InputEventHandlerDelegate* inputEventHandlerDelegate,
              LayoutFieldDelegate* delegate = nullptr,
              KDFont::Size font = KDFont::Size::Large,
              float horizontalAlignment = KDContext::k_alignLeft,
              float verticalAlignment = KDContext::k_alignCenter)
      : WithBlinkingTextCursor<ScrollableView>(parentResponder, &m_contentView,
                                               this),
        EditableField(inputEventHandlerDelegate),
        m_contentView(font, horizontalAlignment, verticalAlignment),
        m_delegate(delegate) {}
  void setDelegates(InputEventHandlerDelegate* inputEventHandlerDelegate,
                    LayoutFieldDelegate* delegate) {
    m_inputEventHandlerDelegate = inputEventHandlerDelegate;
    m_delegate = delegate;
  }
  Poincare::Context* context() const;
  bool isEditing() const override { return m_contentView.isEditing(); }
  void setEditing(bool isEditing) override;
  void clearLayout();
  void scrollToCursor() {
    scrollToBaselinedRect(
        m_contentView.cursorRect(),
        m_contentView.cursor()->layout().baseline(m_contentView.font()));
  }
  bool isEmpty() const { return layout().isEmpty(); }
  Poincare::Layout layout() const {
    return m_contentView.expressionView()->layout();
  }
  bool layoutHasNode() const {
    return m_contentView.expressionView()->layoutHasNode();
  }
  bool addXNTCodePoint(CodePoint defaultXNTCodePoint) override;
  void putCursorOnOneSide(OMG::HorizontalDirection side);
  void setLayout(Poincare::Layout newLayout);
  size_t dumpContent(char* buffer, size_t bufferSize, int* cursorOffset,
                     int* position);

  // ScrollableView
  void setBackgroundColor(KDColor c) override {
    ScrollableView::setBackgroundColor(c);
    m_contentView.setBackgroundColor(c);
  }
  /* Always keep the full margins on a layout field, as it would otherwise lead
   * to weird cropping during edition. */
  float marginPortionTolerance() const override { return 0.f; }

  /* Responder */
  bool handleEventWithText(const char* text, bool indentation = false,
                           bool forceCursorRightOfText = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool handleStoreEvent() override;
  // TODO: factorize with TextField (see TODO of EditableField)
  bool shouldFinishEditing(Ion::Events::Event event) override;

  Poincare::LayoutCursor* cursor() { return m_contentView.cursor(); }
  const ExpressionView* expressionView() const {
    return m_contentView.expressionView();
  }
  ExpressionView* expressionView() { return m_contentView.expressionView(); }

 protected:
  bool linearMode() const {
    return Poincare::Preferences::sharedPreferences->editionMode() ==
           Poincare::Preferences::EditionMode::Edition1D;
  }

 private:
  constexpr static int k_maxNumberOfLayouts = 220;
  static_assert(k_maxNumberOfLayouts == TextField::MaxBufferSize(),
                "Maximal number of layouts in a layout field should be equal "
                "to max number of char in text field");
  void reload(KDSize previousSize);
  virtual bool privateHandleEvent(Ion::Events::Event event,
                                  bool* shouldRedrawLayout);
  bool privateHandleMoveEvent(Ion::Events::Event event,
                              bool* shouldRedrawLayout);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);
  void insertLayoutAtCursor(Poincare::Layout layoutR,
                            bool forceCursorRightOfLayout = false,
                            bool forceCursorLeftOfLayout = false);
  TextCursorView* textCursorView() override {
    return m_contentView.textCursorView();
  }

  class ContentView : public View {
   public:
    ContentView(KDFont::Size font, float horizontalAlignment,
                float verticalAlignment);
    bool isEditing() const { return m_isEditing; }
    // returns True if LayoutField should reload
    bool setEditing(bool isEditing);
    void setBackgroundColor(KDColor c) {
      m_expressionView.setBackgroundColor(c);
    }
    void setCursor(Poincare::LayoutCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged() { layoutCursorSubview(false); }
    KDRect cursorRect() { return m_cursorView.frame(); }
    Poincare::LayoutCursor* cursor() { return &m_cursor; }
    const ExpressionView* expressionView() const { return &m_expressionView; }
    ExpressionView* expressionView() { return &m_expressionView; }
    void clearLayout();
    // View
    KDSize minimalSizeForOptimalDisplay() const override;
    // Selection
    void copySelection(Poincare::Context* context, bool intoStoreMenu);
    KDFont::Size font() const { return m_expressionView.font(); }
    TextCursorView* textCursorView() { return &m_cursorView; }

   private:
    int numberOfSubviews() const override { return 2; }
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void layoutCursorSubview(bool force);
    Poincare::LayoutCursor m_cursor;
    ExpressionViewWithCursor m_expressionView;
    TextCursorView m_cursorView;
    bool m_isEditing;
  };
  ContentView m_contentView;
  LayoutFieldDelegate* m_delegate;
};

}  // namespace Escher

#endif
