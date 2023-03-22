#ifndef ELEMENTS_SUGGESTION_TEXT_FIELD_H
#define ELEMENTS_SUGGESTION_TEXT_FIELD_H

#include <escher/text_field.h>

namespace Elements {

class SuggestionTextField : public Escher::AbstractTextField {
 public:
  SuggestionTextField(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Escher::TextFieldDelegate* delegate);

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

  void setSuggestion(const char* suggestion) {
    m_contentView.setSuggestion(suggestion);
  }
  void commitSuggestion();

 private:
  class ContentView : public Escher::AbstractTextField::ContentView {
   public:
    ContentView(char* textBuffer, size_t textBufferSize, KDFont::Size font,
                float horizontalAlignment, float verticalAlignment,
                KDColor textColor, KDColor backgroundColor);

    // Escher::View
    void drawRect(KDContext* ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;

    const char* suggestion() const { return m_suggestion; }
    const char* suggestionSuffix() const {
      return UTF8Helper::SuffixCaseInsensitiveNoCombining(editedText(),
                                                          m_suggestion);
    }
    void setSuggestion(const char* suggestion);

   private:
    const char* m_suggestion;
  };

  const ContentView* nonEditableContentView() const override {
    return &m_contentView;
  }

  ContentView m_contentView;
};

}  // namespace Elements

#endif
