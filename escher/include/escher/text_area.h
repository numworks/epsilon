#ifndef ESCHER_TEXT_AREA_H
#define ESCHER_TEXT_AREA_H

#include <escher/scrollable_view.h>
#include <escher/text_area_delegate.h>
#include <escher/text_cursor_view.h>
#include <string.h>

class TextArea : public ScrollableView, public ScrollViewDataSource {
public:
  TextArea(Responder * parentResponder, char * textBuffer, size_t textBufferSize,
    TextAreaDelegate * delegate = nullptr, KDText::FontSize size = KDText::FontSize::Large,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);

  void setDelegate(TextAreaDelegate * delegate);

private:
  class Text {
  public:
    Text(char * buffer, size_t bufferSize);

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
      Position(size_t column, size_t line) : m_column(column), m_line(line) {}
      size_t column() const { return m_column; }
      size_t line() const { return m_line; }
    private:
      size_t m_column;
      size_t m_line;
    };

    LineIterator begin() const { return LineIterator(m_buffer); };
    LineIterator end() const { return LineIterator(nullptr); };

    Position span() const;

    void insert(char c, Position p);
    void remove(Position p);
  private:
    size_t indexAtPosition(Position p);
    void insert(char c, size_t index);
    void remove(size_t index);
  private:
    char * m_buffer;
    size_t m_bufferSize;
  };

  class ContentView : public View {
  public:
    ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize size,
      KDColor textColor, KDColor backgroundColor);
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    Text m_text;
    KDText::FontSize m_fontSize;
    KDColor m_textColor;
    KDColor m_backgroundColor;
  };

  ContentView m_contentView;
  TextAreaDelegate * m_delegate;
};

#endif
