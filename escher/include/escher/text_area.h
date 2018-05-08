#ifndef ESCHER_TEXT_AREA_H
#define ESCHER_TEXT_AREA_H

#include <escher/text_input.h>
#include <escher/text_area_delegate.h>
#include <escher/highlighter.h>
#include <assert.h>
#include <string.h>

class TextArea : public TextInput {
public:
  TextArea(Responder * parentResponder, char * textBuffer = nullptr, size_t textBufferSize = 0, Highlighter * highlighter = nullptr, TextAreaDelegate * delegate = nullptr, KDText::FontSize fontSize = KDText::FontSize::Large);
  void setDelegate(TextAreaDelegate * delegate) { m_delegate = delegate; }
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  void setText(char * textBuffer, size_t textBufferSize);
  bool setCursorLocation(int location);
private:
  int indentationBeforeCursor() const;
  bool insertTextWithIndentation(const char * textBuffer, int location);
  TextInputDelegate * delegate() override {
    return m_delegate;
  }
  class Text {
  public:
    Text(char * buffer, size_t bufferSize, Highlighter * highlighter);
    ~Text();
    void setText(char * buffer, size_t bufferSize);
    bool highlight(int location = -1);
    const char * text() const { return const_cast<const char *>(m_buffer); }
    const char * attr() const { return const_cast<const char *>(m_attr_buffer); }
    class Line {
    public:
      Line(const char * text, const char * attr);
      const char * text() const { return m_text; }
      const char * attr() const { return m_attr; }
      size_t length() const { return m_length; }
      bool contains(const char * c) const;
    private:
      const char * m_text;
      const char * m_attr;
      size_t m_length;
    };

    class LineIterator {
    public:
      LineIterator(const char * text, const char * attr) : m_line(text, attr) {}
      Line operator*() { return m_line; }
      LineIterator& operator++();
      bool operator!=(const LineIterator& it) const { return m_line.text() != it.m_line.text(); }
    private:
      Line m_line;
    };

    class Position {
    public:
      Position(int column, int line) : m_column(column), m_line(line) {}
      int column() const { return m_column; }
      int line() const { return m_line; }
    private:
      int m_column;
      int m_line;
    };

    LineIterator begin() const { return LineIterator(m_buffer, m_attr_buffer); };
    LineIterator end() const { return LineIterator(nullptr, nullptr); };

    Position span() const;

    Position positionAtIndex(size_t index) const;
    size_t indexAtPosition(Position p);

    void setAttr(char a, size_t index);
    void insertChar(char c, size_t index);
    char removeChar(size_t index);
    size_t removeRemainingLine(size_t index, int direction);
    char operator[](size_t index) {
      assert(index < m_bufferSize);
      return m_buffer[index];
    }
    size_t bufferSize() const {
      return m_bufferSize;
    }
    size_t textLength() const {
      return strlen(m_buffer);
    }
  private:
    char * m_buffer;
    char * m_attr_buffer;
    size_t m_bufferSize;
    Highlighter * m_highlighter;
  };

  class ContentView : public TextInput::ContentView {
  public:
    ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize size, Highlighter * highlighter);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    void setText(char * textBuffer, size_t textBufferSize);
    const char * text() const override;
    Text * getText() { return &m_text; }
    size_t editedTextLength() const override {
      return m_text.textLength();
    }
    bool insertTextAtLocation(const char * text, int location) override;
    void moveCursorGeo(int deltaX, int deltaY);
    bool removeChar() override;
    bool removeEndOfLine() override;
    bool removeStartOfLine();
    void setCursorLocation(int cursorLocation);
  private:
    KDRect characterFrameAtIndex(size_t index) const override;
    Text m_text;
  };
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
  TextAreaDelegate * m_delegate;
};

#endif
