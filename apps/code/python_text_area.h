#ifndef CODE_PYTHON_TEXT_AREA_H
#define CODE_PYTHON_TEXT_AREA_H

#include <escher/text_area.h>

namespace Code {

class App;

class PythonTextArea : public TextArea {
public:
  enum class AutocompletionType : uint8_t {
    EndOfIdentifier,
    MiddleOfIdentifier,
    NoIdentifier
  };
  PythonTextArea(Responder * parentResponder, App * pythonDelegate, const KDFont * font) :
    TextArea(parentResponder, &m_contentView, font),
    m_contentView(pythonDelegate, font),
    m_autocompletionResultIndex(0)
  {
  }
  void loadSyntaxHighlighter() { m_contentView.loadSyntaxHighlighter(); }
  void unloadSyntaxHighlighter() { m_contentView.unloadSyntaxHighlighter(); }
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false, bool shouldRemoveLastCharacter = false) override;
  /* autocompletionType returns:
   * - EndOfIdentifier if there is currently autocompletion, or if the cursor is
   *   at the end of an identifier,
   * - MiddleOfIdentifier is the cursor is in the middle of an identifier,
   * - No identifier otherwise.
   * The autocompletionLocation can be provided with autocompletionLocation, or
   * retrieved with autocompletionLocationBeginning and autocompletionLocationEnd. */
  AutocompletionType autocompletionType(const char * autocompletionLocation = nullptr, const char ** autocompletionLocationBeginning = nullptr, const char ** autocompletionLocationEnd = nullptr) const;
  bool isAutocompleting() const { return m_contentView.isAutocompleting(); }
protected:
  class ContentView : public TextArea::ContentView {
  public:
    ContentView(App * pythonDelegate, const KDFont * font) :
      TextArea::ContentView(font),
      m_pythonDelegate(pythonDelegate),
      m_autocomplete(false),
      m_autocompletionEnd(nullptr)
    {
    }
    App * pythonDelegate() { return m_pythonDelegate; }
    void setAutocompleting(bool autocomplete) { m_autocomplete = autocomplete; }
    bool isAutocompleting() const { return m_autocomplete; }
    const char * autocompletionEnd() const { assert(m_autocomplete); return m_autocompletionEnd; }
    void setAutocompletionEnd(const char * end) { m_autocompletionEnd = end; }
    const char * textToAutocomplete() const;
    void loadSyntaxHighlighter();
    void unloadSyntaxHighlighter();
    void clearRect(KDContext * ctx, KDRect rect) const override;
    void drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn, const char * selectionStart, const char * selectionEnd) const override;
    KDRect dirtyRectFromPosition(const char * position, bool includeFollowingLines) const override;
  private:
    App * m_pythonDelegate;
    bool m_autocomplete;
    const char * m_autocompletionEnd;
  };
private:
  void removeAutocompletion();
  void removeAutocompletionText(); // Just removes the suggested text, not the autocompletion mode
  void addAutocompletion();
  bool addAutocompletionTextAtIndex(int nextIndex, int * currentIndexToUpdate = nullptr); // Assumes the var box is already loaded
  void cycleAutocompletion(bool downwards);
  void acceptAutocompletion(bool moveCursorToEndOfAutocompletion);
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
  int m_autocompletionResultIndex;
};

}

#endif
