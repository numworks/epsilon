#ifndef CODE_PYTHON_TEXT_AREA_H
#define CODE_PYTHON_TEXT_AREA_H

#include <escher/text_area.h>

namespace Code {

class PythonTextArea : public TextArea {
public:
  PythonTextArea(Responder * parentResponder, KDText::FontSize fontSize) :
    TextArea(parentResponder, &m_contentView, fontSize),
    m_contentView(fontSize)
  {
  }
protected:
  class ContentView : public TextArea::ContentView {
  public:
    ContentView(KDText::FontSize fontSize);
    ~ContentView();
    void clearRect(KDContext * ctx, KDRect rect) const override;
    void drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn) const override;
    KDRect dirtyRectFromCursorPosition(size_t index, bool lineBreak) const override;
  private:
    static constexpr size_t k_pythonHeapSize = 4096;
    char * m_pythonHeap;
  };
private:
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
  char * m_pythonHeap;
};

}

#endif
