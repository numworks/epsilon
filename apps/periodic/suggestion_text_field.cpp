#include "suggestion_text_field.h"

using namespace Escher;

namespace Periodic {

// SuggestionTextField::ContentView

SuggestionTextField::ContentView::ContentView(char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize, KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  AbstractTextField::ContentView(textBuffer, textBufferSize, draftTextBufferSize, font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_suggestion(nullptr)
{}

void SuggestionTextField::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  AbstractTextField::ContentView::drawRect(ctx, rect);
  if (m_suggestion) {
    ctx->drawString(m_suggestion, glyphFrameAtPosition(text(), text() + editedTextLength()).origin(), m_font, Palette::GrayDark, KDColorWhite);
  }
}

KDSize SuggestionTextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize size = AbstractTextField::ContentView::minimalSizeForOptimalDisplay();
  return m_suggestion ? KDSize(size.width() + KDFont::Font(m_font)->stringSize(m_suggestion).width(), size.height()) : size;
}

void SuggestionTextField::ContentView::setSuggestion(const char * suggestion) {
  m_suggestion = suggestion;
  markRectAsDirty(bounds());
}

// SuggestionTextField

SuggestionTextField::SuggestionTextField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate) :
  AbstractTextField(parentResponder, &m_contentView, inputEventHandlerDelegate, delegate),
  m_contentView(nullptr, maxBufferSize(), maxBufferSize(), KDFont::Size::Large, KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite)
{}

}
