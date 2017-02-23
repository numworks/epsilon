#include "term_sum_controller.h"
#include "../../shared/text_field_delegate.h"
#include <assert.h>
#include <math.h>

using namespace Shared;

namespace Sequence {

TermSumController::TermSumController(Responder * parentResponder, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor) :
  ViewController(parentResponder),
  m_contentView(ContentView(graphView)),
  m_graphRange(graphRange),
  m_sequence(nullptr),
  m_cursor(cursor),
  m_cursorView(VerticalCursorView())
{
}

const char * TermSumController::title() const {
  return "Sommes des termes";
}

View * TermSumController::view() {
  return &m_contentView;
}

void TermSumController::viewWillAppear() {
  m_contentView.graphView()->setVerticalCursor(true);
  m_contentView.graphView()->setCursorView(&m_cursorView);
  m_contentView.graphView()->setBannerView(nullptr);
  m_contentView.graphView()->selectMainView(true);
  m_contentView.graphView()->reload();
  m_contentView.layoutSubviews();
}

bool TermSumController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    if (step > 0) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(roundf(m_cursor->x()-1.0f))) {
      m_contentView.graphView()->reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Right) {
    if (moveCursorHorizontallyToPosition(roundf(m_cursor->x()+1.0f))) {
      m_contentView.graphView()->reload();
      return true;
    }
    return false;
  }
  if (event.hasText() && event.text()[0] >= '0' && event.text()[0] <= '9') {
    if (step > 0 && event.text()[0]-'0' < m_cursor->x()) {
      return false;
    }
    if (moveCursorHorizontallyToPosition(event.text()[0]-'0')) {
      m_contentView.graphView()->reload();
      return true;
    }
    return false;
  }

  return false;
}

bool TermSumController::moveCursorHorizontallyToPosition(int position) {
  if (position < 0) {
    return false;
  }
  float x = position;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  if (m_sequence == nullptr) {
    return false;
  }
  float y = m_sequence->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

void TermSumController::setSequence(Sequence * sequence) {
  m_sequence = sequence;
}

/* Content View */

TermSumController::ContentView::ContentView(GraphView * graphView) :
  m_graphView(graphView)
{
}

int TermSumController::ContentView::numberOfSubviews() const {
  return 2;
}

View * TermSumController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_graphView;
  }
  return &m_legendView;
}

void TermSumController::ContentView::layoutSubviews() {
  m_graphView->setFrame(bounds());
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight));
}

GraphView * TermSumController::ContentView::graphView() {
  return m_graphView;
}

/* Legend View */

TermSumController::ContentView::LegendView::LegendView() :
  m_sum(ExpressionView(0.0f, 0.5f, KDColorBlack, Palette::GreyBright)),
  m_legend(PointerTextView(KDText::FontSize::Small, "SELECTIONNER LE PREMIER TERME", 0.0f, 0.5f, KDColorBlack, Palette::GreyBright))
{
}

void TermSumController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::GreyBright);
}

int TermSumController::ContentView::LegendView::numberOfSubviews() const {
  return 2;
}

View * TermSumController::ContentView::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_sum;
  }
  return &m_legend;
}

void TermSumController::ContentView::LegendView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate heigth = bounds().height();
  m_sum.setFrame(KDRect(0, 0, width/2, heigth));
  m_legend.setFrame(KDRect(width/2, 0, width/2, heigth));
}

}
