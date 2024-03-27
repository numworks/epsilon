#ifndef ESCHER_LAYOUT_FIELD_H
#define ESCHER_LAYOUT_FIELD_H

#include <escher/editable_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/layout_view.h>
#include <escher/text_field.h>
#include <kandinsky/point.h>
#include <poincare/preferences.h>

namespace Escher {

class LayoutField : public EditableField {
 public:
  LayoutField(Responder* parentResponder,
              LayoutFieldDelegate* delegate = nullptr,
              KDGlyph::Format format = {});
  void setDelegate(LayoutFieldDelegate* delegate);
  PoincareJ::Context* context() const;
  bool isEditing() const { return m_contentView.isEditing(); }
  void setEditing(bool isEditing);
  void clearLayout();
  void clearAndSetEditing(bool isEditing);
  void scrollToCursor();

  bool isEmpty() const { return layout().isEmpty(); }
  Poincare::Layout layout() const {
    return m_contentView.layoutView()->layout();
  }
  bool layoutHasNode() const {
    return m_contentView.layoutView()->layoutHasNode();
  }
  void putCursorOnOneSide(OMG::HorizontalDirection side);
  void setLayout(Poincare::Layout newLayout);
  size_t dumpContent(char* buffer, size_t bufferSize, int* cursorOffset,
                     int* position);
  void insertLayoutAtCursor(Poincare::JuniorLayout layoutR,
                            bool forceCursorRightOfLayout = false,
                            bool forceCursorLeftOfLayout = false);

  // ScrollableView
  void setBackgroundColor(KDColor c) override;
  /* Always keep the full margins on a layout field, as it would otherwise lead
   * to weird cropping during edition. */
  float marginPortionTolerance() const override { return 0.f; }
  using ScrollView::layoutSubviews;

  /* Responder */
  bool handleEventWithText(const char* text, bool indentation = false,
                           bool forceCursorRightOfText = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  bool handleStoreEvent() override;
  void didBecomeFirstResponder() override;

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

  PoincareJ::LayoutBufferCursor* cursor() { return m_contentView.cursor(); }
  const LayoutView* layoutView() const { return m_contentView.layoutView(); }
  LayoutView* layoutView() { return m_contentView.layoutView(); }

  /* Warning: this function is VERY dangerous! Indeed: sometimes the
   * m_layoutField might overflow the m_textBuffer once serialized
   * and still have been accepted before because the model can hold a longer
   * buffer. This is the case in the application 'Calculation' and we do not
   * use text() there... TODO: change text() for fillTextInBuffer?*/
  const char* text();
  void setText(const char* text);
  bool inputViewHeightDidChange();
  void reload();
  void restoreContent(const char* buffer, size_t size, int* cursorOffset,
                      int* position);
  void setTextEditionBuffer(char* buffer, size_t bufferSize);

  virtual KDCoordinate inputViewHeight() const;

 protected:
  bool linearMode() const {
    return Poincare::Preferences::SharedPreferences()->editionMode() ==
           Poincare::Preferences::EditionMode::Edition1D;
  }
  bool insertText(const char* text, bool indentation = false,
                  bool forceCursorRightOfText = false);
  void reload(KDSize previousSize);

 private:
  constexpr static int k_maxNumberOfLayouts = 220;
  constexpr static KDCoordinate k_minimalHeight =
      KDFont::GlyphHeight(KDFont::Size::Large);
  static_assert(k_maxNumberOfLayouts == TextField::MaxBufferSize(),
                "Maximal number of layouts in a layout field should be equal "
                "to max number of char in text field");
  bool privateHandleEvent(Ion::Events::Event event, bool* layoutDidChange,
                          bool* shouldUpdateCursor);
  size_t getTextFromEvent(Ion::Events::Event event, char* buffer,
                          size_t bufferSize);
  bool handleMoveEvent(Ion::Events::Event event, bool* layoutDidChange);
  bool didHandleEvent(bool didHandleEvent, bool layoutDidChange,
                      bool shouldUpdateCursor, KDSize previousSize);
  void scrollToBaselinedRect(KDRect rect, KDCoordinate baseline);
  TextCursorView::CursorFieldView* cursorCursorFieldView() override {
    return &m_contentView;
  }
  bool prepareToEdit() override;
  bool findXNT(char* buffer, size_t bufferSize, int xntIndex,
               size_t* cycleSize) override;
  void removePreviousXNT() override;

  class ContentView : public TextCursorView::CursorFieldView {
   public:
    ContentView(KDGlyph::Format format);
    bool isEditing() const { return m_isEditing; }
    // returns True if LayoutField should reload
    bool setEditing(bool isEditing);
    void setBackgroundColor(KDColor c) { m_layoutView.setBackgroundColor(c); }
    void setCursor(PoincareJ::LayoutBufferCursor cursor) { m_cursor = cursor; }
    void cursorPositionChanged() { layoutCursorSubview(false); }
    PoincareJ::LayoutBufferCursor* cursor() { return &m_cursor; }
    const LayoutView* layoutView() const { return &m_layoutView; }
    LayoutView* layoutView() { return &m_layoutView; }
    void clearLayout();
    // View
    KDSize minimalSizeForOptimalDisplay() const override;
    // Selection
    void copySelection(PoincareJ::Context* context, bool intoStoreMenu);
    KDFont::Size font() const { return m_layoutView.font(); }

    KDRect cursorRect() const override;

   private:
    int numberOfSubviews() const override {
      return 1 + TextCursorView::CursorFieldView::numberOfSubviews();
    }
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    PoincareJ::LayoutBufferCursor m_cursor;
    LayoutViewWithCursor m_layoutView;
    bool m_isEditing;
  };

  ContentView m_contentView;
  LayoutFieldDelegate* m_delegate;
  KDCoordinate m_inputViewMemoizedHeight;
  char* m_draftBuffer;
  size_t m_draftBufferSize;
};

}  // namespace Escher

#endif
