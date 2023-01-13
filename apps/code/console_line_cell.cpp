#include "console_line_cell.h"
#include "console_controller.h"
#include <kandinsky/point.h>
#include <kandinsky/coordinate.h>
#include <apps/i18n.h>
#include <apps/global_preferences.h>

using namespace Escher;

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
  ctx->drawString(m_line->text(), KDPointZero, GlobalPreferences::sharedGlobalPreferences()->font(), textColor(m_line), defaultBackgroundColor());
}

KDSize ConsoleLineCell::ScrollableConsoleLineView::ConsoleLineView::minimalSizeForOptimalDisplay() const {
  return m_line ? KDFont::Font(GlobalPreferences::sharedGlobalPreferences()->font())->stringSize(m_line->text()) : KDSizeZero;
}

ConsoleLineCell::ScrollableConsoleLineView::ScrollableConsoleLineView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_consoleLineView, this)
{
  setDecoratorType(Escher::ScrollView::Decorator::Type::Arrows);
  setDecoratorFont(GlobalPreferences::sharedGlobalPreferences()->font());
  setBackgroundColor(KDColorWhite);
}

ConsoleLineCell::ConsoleLineCell(Responder * parentResponder) :
  HighlightCell(),
  Responder(parentResponder),
  m_promptView(GlobalPreferences::sharedGlobalPreferences()->font(),
               I18n::Message::ConsolePrompt, KDContext::k_alignLeft, KDContext::k_alignCenter),
  m_scrollableView(this)
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

void ConsoleLineCell::layoutSubviews(bool force) {
  if (m_line.isCommand()) {
    KDSize promptSize = KDFont::Font(GlobalPreferences::sharedGlobalPreferences()->font())->stringSize(I18n::translate(I18n::Message::ConsolePrompt));
    m_promptView.setFrame(KDRect(KDPointZero, promptSize.width(), bounds().height()), force);
    m_scrollableView.setFrame(KDRect(KDPoint(promptSize.width(), 0), bounds().width() - promptSize.width(), bounds().height()), force);
    return;
  }
  assert(m_line.isResult());
  m_promptView.setFrame(KDRectZero, force);
  m_scrollableView.setFrame(bounds(), force);
}

void ConsoleLineCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_scrollableView);
}

}
