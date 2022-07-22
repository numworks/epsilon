#ifndef PERIODIC_SUGGESTION_TEXT_FIELD_H
#define PERIODIC_SUGGESTION_TEXT_FIELD_H

#include <escher/text_field.h>

namespace Periodic {

class SuggestionTextField : public Escher::AbstractTextField {
public:
  SuggestionTextField(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::TextFieldDelegate * delegate);

  void setSuggestion(const char * suggestion) { m_contentView.setSuggestion(suggestion); }

private:
  class ContentView : public Escher::AbstractTextField::ContentView {
  public:
    ContentView(char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize, KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor);

    // Escher::View
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;

    void setSuggestion(const char * suggestion);

  private:
    const char * m_suggestion;
  };

  const ContentView * nonEditableContentView() const { return &m_contentView; }

  ContentView m_contentView;
};

}

#endif
