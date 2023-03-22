#ifndef CODE_CONSOLE_LINE_CELL_H
#define CODE_CONSOLE_LINE_CELL_H

#include <assert.h>
#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scrollable_view.h>

#include "console_line.h"

namespace Code {

class ConsoleLineCell : public Escher::HighlightCell, public Escher::Responder {
 public:
  ConsoleLineCell(Escher::Responder* parentResponder = nullptr);
  void setLine(ConsoleLine line);

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;
  Escher::Responder* responder() override { return this; }
  const char* text() const override { return m_line.text(); }
  /* View */
  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  /* Responder */
  void didBecomeFirstResponder() override;

 private:
  class ScrollableConsoleLineView
      : public Escher::ScrollableView<Escher::ScrollView::ArrowDecorator>,
        public Escher::ScrollViewDataSource {
   public:
    class ConsoleLineView : public HighlightCell {
     public:
      ConsoleLineView();
      void setLine(ConsoleLine* line);
      void drawRect(KDContext* ctx, KDRect rect) const override;
      KDSize minimalSizeForOptimalDisplay() const override;

     private:
      ConsoleLine* m_line;
    };

    ScrollableConsoleLineView(Escher::Responder* parentResponder);
    ConsoleLineView* consoleLineView() { return &m_consoleLineView; }

   private:
    ConsoleLineView m_consoleLineView;
  };
  static KDColor textColor(ConsoleLine* line) {
    return line->isFromCurrentSession() ? KDColorBlack
                                        : Escher::Palette::GrayDark;
  }
  Escher::MessageTextView m_promptView;
  ScrollableConsoleLineView m_scrollableView;
  ConsoleLine m_line;
};

}  // namespace Code

#endif
