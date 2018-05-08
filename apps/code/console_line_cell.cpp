#include "console_line_cell.h"
#include "console_controller.h"
#include <kandinsky/point.h>
#include <kandinsky/coordinate.h>
#include <apps/i18n.h>

namespace Code {

ConsoleLineCell::ScrollableConsoleLineView::ConsoleLineView::ConsoleLineView() :
  HighlightCell(),
  m_line(nullptr)
{
}

void ConsoleLineCell::ScrollableConsoleLineView::ConsoleLineView::setLine(ConsoleLine * line) {
  m_line = line;
}

void ConsoleLineCell::ScrollableConsoleLineView::ConsoleLineView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  ctx->drawString(m_line->text(), KDPointZero, ConsoleController::k_fontSize, textColor(m_line), isHighlighted()? Palette::Select : KDColorWhite);
}

KDSize ConsoleLineCell::ScrollableConsoleLineView::ConsoleLineView::minimalSizeForOptimalDisplay() const {
  return KDText::stringSize(m_line->text(), ConsoleController::k_fontSize);
}

ConsoleLineCell::ScrollableConsoleLineView::ScrollableConsoleLineView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_consoleLineView, this),
  m_consoleLineView()
{
}

KDSize ConsoleLineCell::ScrollableConsoleLineView::minimalSizeForOptimalDisplay() const {
  return m_consoleLineView.minimalSizeForOptimalDisplay();
}

ConsoleLineCell::ConsoleLineCell(Responder * parentResponder) :
  HighlightCell(),
  Responder(parentResponder),
  m_promptView(ConsoleController::k_fontSize, I18n::Message::ConsolePrompt, 0, 0.5),
  m_scrollableView(this),
  m_line()
{
}

void ConsoleLineCell::setLine(ConsoleLine line) {
  m_line = line;
  m_scrollableView.consoleLineView()->setLine(&m_line);
  m_promptView.setTextColor(textColor(&m_line));
  reloadCell();
}

void ConsoleLineCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_scrollableView.consoleLineView()->setHighlighted(highlight);
}

void ConsoleLineCell::reloadCell() {
  layoutSubviews();
  HighlightCell::reloadCell();
  m_scrollableView.reloadScroll();
}

int ConsoleLineCell::numberOfSubviews() const {
  if (m_line.isCommand()) {
    return 2;
  }
  assert(m_line.isResult());
  return 1;
}

View * ConsoleLineCell::subviewAtIndex(int index) {
  if (m_line.isCommand()) {
    assert(index >= 0 && index < 2);
    View * views[] = {&m_promptView, &m_scrollableView};
    return views[index];
  }
  assert(m_line.isResult());
  assert(index == 0);
  return &m_scrollableView;
}

void ConsoleLineCell::layoutSubviews() {
  if (m_line.isCommand()) {
    KDSize promptSize = KDText::stringSize(I18n::translate(I18n::Message::ConsolePrompt), ConsoleController::k_fontSize);
    m_promptView.setFrame(KDRect(KDPointZero, promptSize.width(), bounds().height()));
    m_scrollableView.setFrame(KDRect(KDPoint(promptSize.width(), 0), bounds().width() - promptSize.width(), bounds().height()));
    return;
  }
  assert(m_line.isResult());
  m_promptView.setFrame(KDRectZero);
  m_scrollableView.setFrame(bounds());
}

void ConsoleLineCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_scrollableView);
}

}
