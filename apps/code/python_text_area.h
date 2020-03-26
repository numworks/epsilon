#ifndef CODE_PYTHON_TEXT_AREA_H
#define CODE_PYTHON_TEXT_AREA_H

#include <escher/text_area.h>

namespace Code {

class App;

class PythonTextArea : public TextArea {
public:
  PythonTextArea(Responder * parentResponder, App * pythonDelegate, const KDFont * font) :
    TextArea(parentResponder, &m_contentView, font),
    m_contentView(pythonDelegate, font)
  {
  }
  void loadSyntaxHighlighter() { m_contentView.loadSyntaxHighlighter(); }
  void unloadSyntaxHighlighter() { m_contentView.unloadSyntaxHighlighter(); }
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  bool isAutocompleting() const { return m_contentView.isAutocompleting(); }
protected:
  class ContentView : public TextArea::ContentView {
  public:
    ContentView(App * pythonDelegate, const KDFont * font) :
      TextArea::ContentView(font),
      m_pythonDelegate(pythonDelegate),
      m_autocomplete(false)
    {
    }
    App * pythonDelegate() { return m_pythonDelegate; }
    void setAutocompleting(bool autocomplete) { m_autocomplete = autocomplete; }
    bool isAutocompleting() const { return m_autocomplete; }
    const char * textToAutocomplete() const;
    void loadSyntaxHighlighter();
    void unloadSyntaxHighlighter();
    void clearRect(KDContext * ctx, KDRect rect) const override;
    void drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn, const char * selectionStart, const char * selectionEnd) const override;
    KDRect dirtyRectFromPosition(const char * position, bool includeFollowingLines) const override;
  private:
    App * m_pythonDelegate;
    bool m_autocomplete;
  };
private:
  void removeAutocompletion();
  void addAutocompletion();
  void acceptAutocompletion(bool moveCursorToEndOfAutocompletion);
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
};

}

#endif
