#include <escher/palette.h>
#include <escher/scroll_view.h>

#include <new>
extern "C" {
#include <assert.h>
}
#include <algorithm>

namespace Escher {

ScrollView::ScrollView(View *contentView, ScrollViewDataSource *dataSource)
    : View(),
      m_contentView(contentView),
      m_dataSource(dataSource),
      m_topMargin(0),
      m_rightMargin(0),
      m_bottomMargin(0),
      m_leftMargin(0),
      m_excessWidth(0),
      m_excessHeight(0),
      m_innerView(this),
      m_backgroundColor(Palette::WallScreen) {
  assert(m_dataSource != nullptr);
  setDecoratorType(Decorator::Type::Bars);
}

KDSize ScrollView::minimalSizeForOptimalDisplay() const {
  KDSize contentSize = m_contentView->minimalSizeForOptimalDisplay();
  KDCoordinate width = contentSize.width() + m_leftMargin + m_rightMargin;
  KDCoordinate height = contentSize.height() + m_topMargin + m_bottomMargin;

  /* Crop right or bottom margins if content fits without a portion of them.
   * With a 0.0 tolerance, right and bottom margin is never cropped.
   * With a 0.8 tolerance, at most 80% of right or bottom margin can be cropped.
   * With a 1.0 tolerance, right or bottom margin can be entirely cropped. */
  KDCoordinate excessWidth = width - bounds().width();
  if (excessWidth > 0 &&
      excessWidth <= marginPortionTolerance() * m_rightMargin) {
    width -= excessWidth;
    m_excessWidth = excessWidth;
  } else {
    m_excessWidth = 0;
  }
  KDCoordinate excessHeight = height - bounds().height();
  if (excessHeight > 0 &&
      excessHeight <= marginPortionTolerance() * m_bottomMargin) {
    height -= excessHeight;
    m_excessHeight = excessHeight;
  } else {
    m_excessHeight = 0;
  }

  return KDSize(width, height);
}

void ScrollView::setTopMargin(KDCoordinate m) {
  KDCoordinate translation = m - m_topMargin;
  m_topMargin = m;
  if (translation != 0 && contentOffset().y() != 0) {
    setContentOffset(KDPoint(contentOffset().x(),
                             std::max(0, contentOffset().y() + translation)));
  }
}

void ScrollView::setLeftMargin(KDCoordinate m) {
  KDCoordinate translation = m - m_leftMargin;
  m_leftMargin = m;
  if (translation != 0 && contentOffset().x() != 0) {
    setContentOffset(KDPoint(std::max(0, contentOffset().x() + translation),
                             contentOffset().y()));
  }
}

void ScrollView::setMargins(KDCoordinate top, KDCoordinate right,
                            KDCoordinate bottom, KDCoordinate left) {
  setTopMargin(top);
  setRightMargin(right);
  setBottomMargin(bottom);
  setLeftMargin(left);
}

void ScrollView::scrollToContentPoint(KDPoint p, bool allowOverscroll) {
  if (!allowOverscroll && !m_contentView->bounds().contains(p)) {
    return;
  }

  KDRect visibleRect = visibleContentRect();

  if (visibleRect.width() < 0 || visibleRect.height() < 0) {
    return;
  }

  KDCoordinate offsetX = 0;
  KDCoordinate offsetY = 0;

  if (visibleRect.left() > p.x()) {
    offsetX = p.x() - visibleRect.left();
  }
  if (visibleRect.right() < p.x()) {
    offsetX = p.x() - visibleRect.right();
  }
  if (visibleRect.top() > p.y()) {
    offsetY = p.y() - visibleRect.top();
  }
  if (visibleRect.bottom() < p.y()) {
    offsetY = p.y() - visibleRect.bottom();
  }
  if (offsetX != 0 || offsetY != 0) {
    setContentOffset(contentOffset().translatedBy(KDPoint(offsetX, offsetY)));
  }

  // Handle cases when the size of the view has decreased.
  setContentOffset(KDPoint(
      std::min(contentOffset().x(),
               std::max<KDCoordinate>(
                   minimalSizeForOptimalDisplay().width() - bounds().width(),
                   KDCoordinate(0))),
      std::min(contentOffset().y(),
               std::max<KDCoordinate>(
                   minimalSizeForOptimalDisplay().height() - bounds().height(),
                   KDCoordinate(0)))));
}

void ScrollView::scrollToContentRect(KDRect rect, bool allowOverscroll) {
  KDRect visibleRect = visibleContentRect();
  /* Compute the farthest corner of the rect to scroll to.
   * By scrolling to it, the whole rect should be displayed.
   */
  KDPoint farthestCorner = KDPointZero;
  if (rect.top() >= visibleRect.top()) {
    if (rect.left() >= visibleRect.left()) {
      farthestCorner = rect.bottomRight();
    } else {
      farthestCorner = rect.bottomLeft();
    }
  } else {
    if (rect.left() >= visibleRect.left()) {
      farthestCorner = rect.topRight();
    } else {
      farthestCorner = rect.topLeft();
    }
  }
  /* We check that we can display the whole rect. If we can't, we focus
   * the scroll to the closest part of the rect. */
  if (visibleRect.height() < rect.height()) {
    // The visible rect is too small to display 'rect'
    if (rect.top() >= visibleRect.top()) {
      // We scroll to display the top part of rect
      farthestCorner =
          KDPoint(farthestCorner.x(), rect.top() + visibleRect.height() - 1);
    } else {
      // We scroll to display the bottom part of rect
      farthestCorner =
          KDPoint(farthestCorner.x(), rect.bottom() - visibleRect.height());
    }
  }
  if (visibleRect.width() < rect.width()) {
    // The visible rect is too small to display 'rect'
    if (rect.left() >= visibleRect.left()) {
      // We scroll to display the left part of rect
      farthestCorner =
          KDPoint(rect.left() + visibleRect.width() - 1, farthestCorner.y());
    } else {
      // We scroll to display the right part of rect
      farthestCorner =
          KDPoint(rect.right() - visibleRect.width(), farthestCorner.y());
    }
  }
  scrollToContentPoint(farthestCorner, allowOverscroll);
}

KDRect ScrollView::visibleContentRect() {
  return KDRect(
      contentOffset().x(), contentOffset().y(),
      bounds().width() - m_leftMargin - m_rightMargin + m_excessWidth,
      bounds().height() - m_topMargin - m_bottomMargin + m_excessHeight);
}

void ScrollView::layoutSubviews(bool force) {
  if (bounds().isEmpty()) {
    return;
  }
  KDRect r1 = KDRectZero;
  KDRect r2 = KDRectZero;
  KDRect innerFrame = decorator()->layoutIndicators(
      this, minimalSizeForOptimalDisplay(), contentOffset(), bounds(), &r1, &r2,
      force, m_dataSource->delegate());
  if (!r1.isEmpty()) {
    markRectAsDirty(r1);
  }
  if (!r2.isEmpty()) {
    markRectAsDirty(r2);
  }
  setChildFrame(&m_innerView, innerFrame, force);
  KDPoint absoluteOffset = contentOffset().opposite().translatedBy(
      KDPoint(m_leftMargin - innerFrame.x(), m_topMargin - innerFrame.y()));
  KDRect contentFrame = KDRect(absoluteOffset, contentSize());
  m_innerView.setChildFrame(m_contentView, contentFrame, force);
}

void ScrollView::setContentOffset(KDPoint offset, bool forceRelayout) {
  if (m_dataSource->setOffset(offset) || forceRelayout) {
    layoutSubviews();
  }
}

void ScrollView::InnerView::drawRect(KDContext *ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate offsetX =
      m_scrollView->contentOffset().x() + relativeFrame().x();
  KDCoordinate offsetY =
      m_scrollView->contentOffset().y() + relativeFrame().y();
  KDCoordinate contentHeight = m_scrollView->m_contentView->bounds().height();
  KDCoordinate contentWidth = m_scrollView->m_contentView->bounds().width();
  // Draw top margin
  ctx->fillRect(KDRect(0, 0, width, m_scrollView->m_topMargin - offsetY),
                m_scrollView->m_backgroundColor);
  // Draw bottom margin
  ctx->fillRect(
      KDRect(0, contentHeight + m_scrollView->m_topMargin - offsetY, width,
             height - contentHeight - m_scrollView->m_topMargin + offsetY),
      m_scrollView->m_backgroundColor);
  // Draw left margin
  ctx->fillRect(KDRect(0, 0, m_scrollView->m_leftMargin - offsetX, height),
                m_scrollView->m_backgroundColor);
  // Draw right margin
  ctx->fillRect(
      KDRect(contentWidth + m_scrollView->m_leftMargin - offsetX, 0,
             width - contentWidth - m_scrollView->m_leftMargin + offsetX,
             height),
      m_scrollView->m_backgroundColor);
}

View *ScrollView::BarDecorator::indicatorAtIndex(int index) {
  if (index == 1) {
    return &m_verticalBar;
  }
  assert(index == 2);
  return &m_horizontalBar;
}

KDRect ScrollView::BarDecorator::layoutIndicators(
    View *parent, KDSize content, KDPoint offset, KDRect frame,
    KDRect *dirtyRect1, KDRect *dirtyRect2, bool force,
    ScrollViewDelegate *delegate) {
  bool hBarWasVisible = m_horizontalBar.visible();
  bool vBarWasVisible = m_verticalBar.visible();
  bool hBarIsVisible, vBarIsVisible;
  if (!(delegate && delegate->updateBarIndicator(false, &hBarIsVisible))) {
    hBarIsVisible =
        m_horizontalBar.update(content.width(), offset.x(), frame.width());
  }
  if (!(delegate && delegate->updateBarIndicator(true, &vBarIsVisible))) {
    vBarIsVisible =
        m_verticalBar.update(content.height(), offset.y(), frame.height());
  }

  KDCoordinate hBarFrameBreadth = k_barsFrameBreadth * hBarIsVisible;
  KDCoordinate vBarFrameBreadth = k_barsFrameBreadth * vBarIsVisible;

  // Mark the vertical and horizontal rects as dirty id needed
  *dirtyRect1 = hBarWasVisible == hBarIsVisible
                    ? KDRectZero
                    : KDRect(frame.width() - k_barsFrameBreadth, 0,
                             k_barsFrameBreadth, frame.height());
  *dirtyRect2 = vBarWasVisible == vBarIsVisible
                    ? KDRectZero
                    : KDRect(0, frame.height() - k_barsFrameBreadth,
                             frame.width(), k_barsFrameBreadth);

  /* If the two indicators are visible, we leave an empty rectangle in the right
   * bottom corner. Otherwise, the only indicator uses all the height/width. */
  parent->setChildFrame(
      &m_verticalBar,
      KDRect(frame.width() - vBarFrameBreadth, 0, vBarFrameBreadth,
             frame.height() - hBarFrameBreadth),
      force);
  parent->setChildFrame(
      &m_horizontalBar,
      KDRect(0, frame.height() - hBarFrameBreadth,
             frame.width() - vBarFrameBreadth, hBarFrameBreadth),
      force);
  return frame;
}

View *ScrollView::ArrowDecorator::indicatorAtIndex(int index) {
  switch (index) {
    case 1:
      return &m_rightArrow;
    default:
      assert(index == 2);
      return &m_leftArrow;
  }
}

KDRect ScrollView::ArrowDecorator::layoutIndicators(
    View *parent, KDSize content, KDPoint offset, KDRect frame,
    KDRect *dirtyRect1, KDRect *dirtyRect2, bool force,
    ScrollViewDelegate *delegate) {
  // There is no need to dirty the rects
  KDSize arrowSize = KDFont::GlyphSize(KDFont::Size::Large);
  KDCoordinate rightArrowFrameBreadth =
      arrowSize.width() *
      m_rightArrow.update(offset.x() + frame.width() < content.width());
  KDCoordinate leftArrowFrameBreadth =
      arrowSize.width() * m_leftArrow.update(0 < offset.x());
  parent->setChildFrame(&m_rightArrow,
                        KDRect(frame.width() - rightArrowFrameBreadth, 0,
                               rightArrowFrameBreadth, frame.height()),
                        force);
  parent->setChildFrame(
      &m_leftArrow, KDRect(0, 0, leftArrowFrameBreadth, frame.height()), force);
  return KDRect(frame.x() + leftArrowFrameBreadth, frame.y(),
                frame.width() - leftArrowFrameBreadth - rightArrowFrameBreadth,
                frame.height());
}

void ScrollView::ArrowDecorator::setBackgroundColor(KDColor c) {
  const int indicatorsCount = numberOfIndicators();
  for (int index = 1; index <= indicatorsCount; index++) {
    static_cast<ScrollViewArrow *>(indicatorAtIndex(index))
        ->setBackgroundColor(c);
  }
}

void ScrollView::ArrowDecorator::setFont(KDFont::Size font) {
  m_rightArrow.setFont(font);
  m_leftArrow.setFont(font);
}

ScrollView::Decorators::Decorators() {
  /* We need to initiate the Union at construction to avoid destructing an
   * uninitialized object when changing the decorator type. */
  new (this) Decorator();
}

ScrollView::Decorators::~Decorators() { activeDecorator()->~Decorator(); }

void ScrollView::Decorators::setActiveDecorator(Decorator::Type t) {
  /* Decorator destructor is virtual so calling ~Decorator() on a Decorator
   * pointer will call the appropriate destructor. */
  activeDecorator()->~Decorator();
  switch (t) {
    case Decorator::Type::Bars:
      new (&m_bars) BarDecorator();
      break;
    case Decorator::Type::Arrows:
      new (&m_arrows) ArrowDecorator();
      break;
    default:
      assert(t == Decorator::Type::None);
      new (&m_none) Decorator();
  }
}

#if ESCHER_VIEW_LOGGING
const char *ScrollView::className() const { return "ScrollView"; }

void ScrollView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " offset=\"" << (int)contentOffset().x() << ","
     << (int)contentOffset().y() << "\"";
}
#endif

}  // namespace Escher
