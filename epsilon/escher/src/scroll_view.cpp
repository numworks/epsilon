#include <escher/palette.h>
#include <escher/scroll_view.h>

#include <new>
extern "C" {
#include <assert.h>
}
#include <algorithm>

namespace Escher {

ScrollView::ScrollView(View* contentView, ScrollViewDataSource* dataSource,
                       Escher::ScrollViewDelegate* scrollViewDelegate)
    : View(),
      m_dataSource(dataSource),
      m_contentView(contentView),
      m_innerView(this),
      m_scrollViewDelegate(scrollViewDelegate),
      m_margins(),
      m_excessWidth(0),
      m_excessHeight(0),
      m_backgroundColor(Palette::WallScreen) {
  assert(m_dataSource != nullptr);
}

KDSize ScrollView::contentSizeWithMargins() const {
  return m_contentView->minimalSizeForOptimalDisplay() + m_margins;
}

KDSize ScrollView::minimalSizeForOptimalDisplay() const {
  KDSize contentSize = contentSizeWithMargins();
  KDCoordinate width = contentSize.width();
  KDCoordinate height = contentSize.height();

  /* Crop right or bottom margins if content fits without a portion of them.
   * With a 0.0 tolerance, right and bottom margin is never cropped.
   * With a 0.8 tolerance, at most 80% of right or bottom margin can be cropped.
   * With a 1.0 tolerance, right or bottom margin can be entirely cropped. */
  KDCoordinate excessWidth = width - bounds().width();
  if (excessWidth > 0 &&
      excessWidth <= marginPortionTolerance() * m_margins.right()) {
    width -= excessWidth;
    m_excessWidth = excessWidth;
  } else {
    m_excessWidth = 0;
  }
  KDCoordinate excessHeight = height - bounds().height();
  if (excessHeight > 0 &&
      excessHeight <= marginPortionTolerance() * m_margins.bottom()) {
    height -= excessHeight;
    m_excessHeight = excessHeight;
  } else {
    m_excessHeight = 0;
  }

  return KDSize(width, height);
}

void ScrollView::scrollToContentPoint(KDPoint p) {
  KDRect visibleRect = visibleContentRect();

  if (visibleRect.isEmpty()) {
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

  KDPoint newOffset = contentOffset().translatedBy(KDPoint(offsetX, offsetY));
  setClippedContentOffset(newOffset);
}

void ScrollView::setClippedContentOffset(KDPoint offset) {
  // Handle cases when the size of the view has decreased.
  KDCoordinate maxOffsetX = std::max<KDCoordinate>(
      minimalSizeForOptimalDisplay().width() - bounds().width(),
      KDCoordinate(0));
  KDCoordinate maxOffsetY = std::max<KDCoordinate>(
      minimalSizeForOptimalDisplay().height() - bounds().height(),
      KDCoordinate(0));
  setContentOffset(KDPoint(std::min(offset.x(), maxOffsetX),
                           std::min(offset.y(), maxOffsetY)));
}

void ScrollView::scrollToContentRect(KDRect rect) {
  if (rect.isEmpty()) {
    return;
  }
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
  scrollToContentPoint(farthestCorner);
}

KDRect ScrollView::visibleContentRect() {
  KDRect visibleRect = visibleRectInBounds();
  assert(bounds().containsRect(visibleRect));
  return KDRect(
      contentOffset().translatedBy(visibleRect.origin()),
      visibleRect.size() + KDSize(m_excessWidth, m_excessHeight) - m_margins);
}

void ScrollView::layoutSubviews(bool force) {
  if (bounds().isEmpty()) {
    return;
  }

  KDRect innerFrame = bounds();
  if (decorator()->layoutBeforeInnerView()) {
    innerFrame = layoutDecorator(force);
  }

  setChildFrame(&m_innerView, innerFrame, force);
  KDPoint offset = contentOffset()
                       .opposite()
                       .translatedBy(m_margins.topLeftPoint())
                       .relativeTo(innerFrame.origin());

  KDRect contentFrame = KDRect(offset, contentSize());
  m_innerView.setChildFrame(m_contentView, contentFrame,
                            force || alwaysForceRelayoutOfContentView());

  if (!decorator()->layoutBeforeInnerView()) {
    layoutDecorator(force);
  }
}

void ScrollView::setContentOffset(KDPoint offset) {
  if (m_dataSource->setOffset(offset)) {
    layoutSubviews();
  }
}

KDRect ScrollView::layoutDecorator(bool force) {
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
  return innerFrame;
}

void ScrollView::InnerView::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDRect contentFrame = relativeChildFrame(m_scrollView->m_contentView);
  KDColor color = m_scrollView->m_backgroundColor;
  // Draw top margin
  KDCoordinate contentTop = contentFrame.origin().y();
  ctx->fillRect(KDRect(0, 0, width, contentTop), color);
  // Draw bottom margin
  KDCoordinate contentBottom = contentTop + contentFrame.size().height();
  ctx->fillRect(KDRect(0, contentBottom, width, height - contentBottom), color);
  // Draw left margin
  KDCoordinate contentLeft = contentFrame.origin().x();
  ctx->fillRect(KDRect(0, 0, contentLeft, height), color);
  // Draw right margin
  KDCoordinate contentRight = contentLeft + contentFrame.size().width();
  ctx->fillRect(KDRect(contentRight, 0, width - contentRight, height), color);
}

View* ScrollView::BarDecorator::indicatorAtIndex(int index) {
  if (index == 1) {
    return &m_verticalBar;
  }
  assert(index == 2);
  return &m_horizontalBar;
}

KDRect ScrollView::BarDecorator::layoutIndicators(
    View* parent, KDSize content, KDPoint offset, KDRect frame,
    KDRect* dirtyRect1, KDRect* dirtyRect2, bool force,
    ScrollViewDataSourceDelegate* delegate) {
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

View* ScrollView::ArrowDecorator::indicatorAtIndex(int index) {
  switch (index) {
    case 1:
      return &m_rightArrow;
    default:
      assert(index == 2);
      return &m_leftArrow;
  }
}

KDRect ScrollView::ArrowDecorator::layoutIndicators(
    View* parent, KDSize content, KDPoint offset, KDRect frame,
    KDRect* dirtyRect1, KDRect* dirtyRect2, bool force,
    ScrollViewDataSourceDelegate* delegate) {
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
  m_rightArrow.setBackgroundColor(c);
  m_leftArrow.setBackgroundColor(c);
}

void ScrollView::ArrowDecorator::setFont(KDFont::Size font) {
  m_rightArrow.setFont(font);
  m_leftArrow.setFont(font);
}

#if ESCHER_VIEW_LOGGING
const char* ScrollView::className() const { return "ScrollView"; }

void ScrollView::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " offset=\"" << (int)contentOffset().x() << ","
     << (int)contentOffset().y() << "\"";
}
#endif

}  // namespace Escher
