#include "test_conclusion_view.h"

#include <apps/i18n.h>
#include <apps/shared/dots.h>
#include <float.h>
#include <kandinsky/font.h>
#include <math.h>

#include "../images/test_failure.h"
#include "../images/test_success.h"

namespace Probability {

TestConclusionView::TestConclusionView() {
  m_textView.setBackgroundColor(Escher::Palette::WallScreen);
  m_icon.setImage(ImageStore::TestSuccess);
}

void TestConclusionView::setType(Type t) {
  I18n::Message m =
      t == Type::Success ? I18n::Message::ConclusionSuccess : I18n::Message::ConclusionFailure;
  m_textView.setMessage(m);
  if (t == Type::Success) {
    m_icon.setImage(ImageStore::TestSuccess);
  } else {
    m_icon.setImage(ImageStore::TestFailure);
  }
  markRectAsDirty(bounds());
}

Escher::View * TestConclusionView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_icon, &m_textView};
  return subviews[i];
}

void TestConclusionView::layoutSubviews(bool force) {
  int iconOriginHeight = (m_frame.height() - k_iconSize) / 2;
  m_icon.setFrame(
      KDRect(KDPoint(k_marginLeft, iconOriginHeight), m_icon.minimalSizeForOptimalDisplay()),
      force);
  m_textView.setFrame(KDRect(KDPoint(k_iconSize + k_marginBetween + k_marginLeft, 0),
                             KDSize(m_frame.width() - k_iconSize, m_frame.height())),
                      force);
}

KDSize TestConclusionView::minimalSizeForOptimalDisplay() const {
  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(iconSize.width() + textSize.width(), fmax(iconSize.height(), textSize.height()));
}

}  // namespace Probability
