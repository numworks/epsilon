#ifndef CALCULATION_JUNIOR_LAYOUT_FIELD_H
#define CALCULATION_JUNIOR_LAYOUT_FIELD_H

#include <escher/editable_field.h>
#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <escher/text_field.h>
#include <kandinsky/point.h>
#include <poincare/preferences.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/render.h>

#include "expression_view.h"
#include "layout_field_delegate.h"

// See TODO in EditableField

namespace CalculationJunior {

class LayoutField
    : public Escher::WithBlinkingTextCursor<
          Escher::ScrollableView<Escher::ScrollView::NoDecorator>>,
      public Escher::ScrollViewDataSource,
      public Escher::EditableField {
  friend class ExpressionField;

 public:
  LayoutField(Escher::Responder* parentResponder,
              Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
              LayoutFieldDelegate* delegate = nullptr,
              KDGlyph::Format format = {})
      : Escher::WithBlinkingTextCursor<
            Escher::ScrollableView<Escher::ScrollView::NoDecorator>>(
            parentResponder, &m_contentView, this),
        Escher::EditableField(inputEventHandlerDelegate),
        m_contentView(format),
        m_delegate(delegate) {}
  void setDelegates(
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      LayoutFieldDelegate* delegate) {
    m_inputEventHandlerDelegate = inputEventHandlerDelegate;
    m_delegate = delegate;
  }
  PoincareJ::Context* context() const;
  bool isEditing() const override { return m_contentView.isEditing(); }
  void setEditing(bool isEditing) override;
  void clearLayout();
  void scrollToCursor() {
    scrollToBaselinedRect(
        m_contentView.cursorRect(),
        PoincareJ::Render::Baseline(m_contentView.cursor()->layout(),
                                    m_contentView.font()));
  }
  bool isEmpty() const { return layout().isEmpty(); }
  PoincareJ::Layout layout() const {
    return m_contentView.expressionView()->getLayout();
  }
  bool layoutHasNode() const {
    return m_contentView.expressionView()->layoutHasNode();
  }
  bool addXNTCodePoint(CodePoint defaultXNTCodePoint);
  void putCursorOnOneSide(OMG::HorizontalDirection side);
  void setLayout(PoincareJ::Layout newLayout);
  size_t dumpContent(char* buffer, size_t bufferSize, int* cursorOffset,
                     int* position);

  // Escher::ScrollableView
  void setBackgroundColor(KDColor c) override {
    Escher::ScrollableView<Escher::ScrollView::NoDecorator>::setBackgroundColor(
        c);
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
  // TODO: factorize with Escher::TextField (see TODO of EditableField)
  bool shouldFinishEditing(Ion::Events::Event event) override;

  PoincareJ::LayoutCursor* cursor() { return m_contentView.cursor(); }
  const ExpressionViewWithCursor* expressionView() const {
    return m_contentView.expressionView();
  }
  ExpressionViewWithCursor* expressionView() {
    return m_contentView.expressionView();
  }

 protected:
  bool linearMode() const {
    return Poincare::Preferences::SharedPreferences()->editionMode() ==
           Poincare::Preferences::EditionMode::Edition1D;
  }

 private:
  constexpr static int k_maxNumberOfLayouts = 220;
  static_assert(k_maxNumberOfLayouts == Escher::TextField::MaxBufferSize(),
                "Maximal number of layouts in a layout field should be equal "
                "to max number of char in text field");
  void reload(KDSize previousSize);
  bool privateHandleEvent(Ion::Events::Event event, bool* shouldRedrawLayout,
                          bool* shouldUpdateCursor);
  bool privateHandleMoveEvent(Ion::Events::Event event,
                              bool* shouldRedrawLayout);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);
  void insertLayoutAtCursor(PoincareJ::Node layoutR,
                            bool forceCursorRightOfLayout = false,
                            bool forceCursorLeftOfLayout = false);
  Escher::TextCursorView* textCursorView() override {
    return m_contentView.textCursorView();
  }

  class ContentView : public Escher::View {
   public:
    ContentView(KDGlyph::Format format);
    bool isEditing() const { return m_isEditing; }
    // returns True if LayoutField should reload
    bool setEditing(bool isEditing);
    void setBackgroundColor(KDColor c) {
      m_expressionView.setBackgroundColor(c);
    }
    void setCursor(PoincareJ::LayoutCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged() { layoutCursorSubview(false); }
    KDRect cursorRect() { return relativeChildFrame(&m_cursorView); }
    PoincareJ::LayoutCursor* cursor() { return &m_cursor; }
    const ExpressionViewWithCursor* expressionView() const {
      return &m_expressionView;
    }
    ExpressionViewWithCursor* expressionView() { return &m_expressionView; }
    void clearLayout();
    // View
    KDSize minimalSizeForOptimalDisplay() const override;
    // Selection
    void copySelection(bool intoStoreMenu);
    KDFont::Size font() const { return m_expressionView.font(); }
    Escher::TextCursorView* textCursorView() { return &m_cursorView; }
    PoincareJ::Node node() { return PoincareJ::Node(m_layoutBuffer); }

   private:
    int numberOfSubviews() const override { return 2; }
    Escher::View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    void layoutCursorSubview(bool force);
    PoincareJ::LayoutCursor m_cursor;
    ExpressionViewWithCursor m_expressionView;
    Escher::TextCursorView m_cursorView;
    bool m_isEditing;
    // Buffer where layout being edited is stored. TODO : refine size
    PoincareJ::TypeBlock m_layoutBuffer[k_maxNumberOfLayouts];
  };
  ContentView m_contentView;
  LayoutFieldDelegate* m_delegate;
};

}  // namespace CalculationJunior

#endif
