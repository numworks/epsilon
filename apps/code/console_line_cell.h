#ifndef CODE_CONSOLE_LINE_CELL_H
#define CODE_CONSOLE_LINE_CELL_H

#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/palette.h>
#include <escher/scrollable_view.h>
#include <escher/scroll_view_data_source.h>
#include <assert.h>

#include "console_line.h"

namespace Code {

class ConsoleLineCell : public HighlightCell, public Responder {
public:
  ConsoleLineCell(Responder * parentResponder = nullptr);
  void setLine(ConsoleLine line);

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;
  Responder * responder() override {
    return this;
  }
  const char * text() const override {
    return m_line.text();
  }
  /* View */
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  /* Responder */
  void didBecomeFirstResponder() override;
private:
  class ScrollableConsoleLineView : public ScrollableView, public ScrollViewDataSource {
  public:
    class ConsoleLineView : public HighlightCell {
    public:
      ConsoleLineView();
      void setLine(ConsoleLine * line);
      void drawRect(KDContext * ctx, KDRect rect) const override;
      KDSize minimalSizeForOptimalDisplay() const override;
    private:
      ConsoleLine * m_line;
    };

    ScrollableConsoleLineView(Responder * parentResponder);
    ConsoleLineView * consoleLineView() { return &m_consoleLineView; }
  private:
    ConsoleLineView m_consoleLineView;
  };
  static KDColor textColor(ConsoleLine * line) {
    return line->isFromCurrentSession() ? KDColorBlack : Palette::GreyDark;
  }
  MessageTextView m_promptView;
  ScrollableConsoleLineView m_scrollableView;
  ConsoleLine m_line;
};

}

#endif
