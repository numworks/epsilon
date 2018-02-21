#ifndef ESCHER_TEXT_AREA_H
#define ESCHER_TEXT_AREA_H

#include <escher/text_input.h>
#include <escher/text_area_delegate.h>
#include <assert.h>
#include <string.h>

class TextArea : public TextInput {
public:
  TextArea(Responder * parentResponder, char * textBuffer = nullptr, size_t textBufferSize = 0, TextAreaDelegate * delegate = nullptr, KDText::FontSize fontSize = KDText::FontSize::Large,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setDelegate(TextAreaDelegate * delegate) { m_delegate = delegate; }
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEventWithText(const char * text, bool indentation = false) override;
  void setText(char * textBuffer, size_t textBufferSize);
private:
  int indentationBeforeCursor() const;
  bool insertTextWithIndentation(const char * textBuffer, int location);
  TextInputDelegate * delegate() override {
    return m_delegate;
  }
  class Text {
  public:
    Text(char * buffer, size_t bufferSize);
    void setText(char * buffer, size_t bufferSize);
    const char * text() const { return const_cast<const char *>(m_buffer); }
    class Line {
    public:
      Line(const char * text);
      const char * text() const { return m_text; }
      size_t length() const { return m_length; }
      bool contains(const char * c) const;
    private:
      const char * m_text;
      size_t m_length;
    };

    class LineIterator {
    public:
      LineIterator(const char * text) : m_line(text) {}
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

    LineIterator begin() const { return LineIterator(m_buffer); };
    LineIterator end() const { return LineIterator(nullptr); };

    Position span() const;

    Position positionAtIndex(size_t index) const;
    size_t indexAtPosition(Position p);

    void insertChar(char c, size_t index);
    char removeChar(size_t index);
    int removeRemainingLine(size_t index, int direction);
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
    size_t m_bufferSize;
  };

  class ContentView : public TextInput::ContentView {
  public:
    ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize size,
      KDColor textColor, KDColor backgroundColor);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    void setText(char * textBuffer, size_t textBufferSize);
    const char * text() const override;
    const Text * getText() const { return &m_text; }
    size_t editedTextLength() const override {
      return m_text.textLength();
    }
    bool insertTextAtLocation(const char * text, int location) override;
    void moveCursorGeo(int deltaX, int deltaY);
    bool removeChar() override;
    bool removeEndOfLine() override;
    bool removeStartOfLine();
  private:
    KDRect characterFrameAtIndex(size_t index) const override;
    Text m_text;
  };
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
  TextAreaDelegate * m_delegate;
};

#endif
