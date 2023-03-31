#include "suggestion_text_field.h"

using namespace Escher;

namespace Elements {

// SuggestionTextField::ContentView

SuggestionTextField::ContentView::ContentView(
    char* textBuffer, size_t textBufferSize, KDFont::Size font,
    float horizontalAlignment, float verticalAlignment, KDColor textColor,
    KDColor backgroundColor)
    : AbstractTextField::ContentView(textBuffer, textBufferSize, font,
                                     horizontalAlignment, verticalAlignment,
                                     textColor, backgroundColor),
      m_suggestion(nullptr) {}

void SuggestionTextField::ContentView::drawRect(KDContext* ctx,
                                                KDRect rect) const {
  AbstractTextField::ContentView::drawRect(ctx, rect);
  if (m_suggestion) {
    assert(strlen(m_suggestion) >= editedTextLength());
    ctx->drawString(
        suggestionSuffix(),
        glyphFrameAtPosition(text(), text() + editedTextLength()).origin(),
        {.glyphColor = Palette::GrayDark,
         .backgroundColor = KDColorWhite,
         .font = m_font});
  }
}

KDSize SuggestionTextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize size = AbstractTextField::ContentView::minimalSizeForOptimalDisplay();
  return m_suggestion
             ? KDSize(size.width() + KDFont::Font(m_font)
                                         ->stringSize(suggestionSuffix())
                                         .width(),
                      size.height())
             : size;
}

void SuggestionTextField::ContentView::setSuggestion(const char* suggestion) {
  m_suggestion = suggestion;
  markRectAsDirty(bounds());
}

// SuggestionTextField

SuggestionTextField::SuggestionTextField(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* delegate)
    : AbstractTextField(parentResponder, &m_contentView,
                        inputEventHandlerDelegate, delegate),
      m_contentView(nullptr, MaxBufferSize(), KDFont::Size::Large,
                    KDGlyph::k_alignLeft, KDGlyph::k_alignCenter, KDColorBlack,
                    KDColorWhite) {}

bool SuggestionTextField::handleEvent(Ion::Events::Event event) {
  if (cursorAtEndOfText() && m_contentView.suggestion() &&
      (event == Ion::Events::Left || event == Ion::Events::ShiftLeft ||
       event == Ion::Events::ShiftUp)) {
    // Dismiss suggestion on Left press
    m_contentView.setSuggestion(nullptr);
    return true;
  }
  bool result = AbstractTextField::handleEvent(event);
  // If a suggestion remains, cursor must be at the end of the text
  assert(!m_contentView.suggestion() || cursorAtEndOfText());
  return result;
}

void SuggestionTextField::commitSuggestion() {
  if (m_contentView.suggestion()) {
    setText(m_contentView.suggestion());
  }
}

}  // namespace Elements
