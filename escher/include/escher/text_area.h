#ifndef ESCHER_TEXT_AREA_H
#define ESCHER_TEXT_AREA_H

#include <escher/text_input.h>
#include <escher/text_area_delegate.h>
#include <assert.h>
#include <string.h>

class TextArea : public TextInput {
public:
  TextArea(Responder * parentResponder, View * contentView, KDText::FontSize fontSize = KDText::FontSize::Large);
  void setDelegate(TextAreaDelegate * delegate) { m_delegate = delegate; }
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) override;
  void setText(char * textBuffer, size_t textBufferSize);

protected:
  int indentationBeforeCursor() const;
  bool insertTextWithIndentation(const char * textBuffer, int location);
  TextInputDelegate * delegate() override {
    return m_delegate;
  }

  class Text {
  public:
    Text(char * buffer, size_t bufferSize) :
      m_buffer(buffer),
      m_bufferSize(bufferSize)
    {
    }
    void setText(char * buffer, size_t bufferSize) {
      m_buffer = buffer;
      m_bufferSize = bufferSize;
    }
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
    size_t m_bufferSize;
  };

  class ContentView : public TextInput::ContentView {
  public:
    ContentView(KDText::FontSize fontSize) :
      TextInput::ContentView(fontSize),
      m_text(nullptr, 0)
    {
    }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void drawStringAt(KDContext * ctx, int line, int column, const char * text, size_t length, KDColor textColor, KDColor backgroundColor) const;
    virtual void drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn) const = 0;
    virtual void clearRect(KDContext * ctx, KDRect rect) const = 0;
    KDSize minimalSizeForOptimalDisplay() const override;
    void setText(char * textBuffer, size_t textBufferSize);
    const char * text() const override { return m_text.text(); }
    size_t editedTextLength() const override { return m_text.textLength(); }
    const Text * getText() const { return &m_text; }
    bool insertTextAtLocation(const char * text, int location) override;
    void moveCursorGeo(int deltaX, int deltaY);
    bool removeChar() override;
    bool removeEndOfLine() override;
    bool removeStartOfLine();
  protected:
    KDRect characterFrameAtIndex(size_t index) const override;
    Text m_text;
  };

  ContentView * contentView() { return static_cast<ContentView *>(TextInput::contentView()); }
private:
  TextAreaDelegate * m_delegate;
};

#endif
