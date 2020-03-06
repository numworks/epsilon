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
protected:
  class ContentView : public TextArea::ContentView {
  public:
    ContentView(App * pythonDelegate, const KDFont * font) :
      TextArea::ContentView(font),
      m_pythonDelegate(pythonDelegate)
    {
    }
    void loadSyntaxHighlighter();
    void unloadSyntaxHighlighter();
    void clearRect(KDContext * ctx, KDRect rect) const override;
    void drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn, const char * selectionStart, const char * selectionEnd) const override;
    KDRect dirtyRectFromPosition(const char * position, bool includeFollowingLines) const override;
  private:
    App * m_pythonDelegate;
  };
private:
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
};

}

#endif
