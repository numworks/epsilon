
#include "pie_banner_view.h"

namespace Statistics::Categorical {

static void FillWithMessageAndColon(Escher::TextView* textView,
                                    I18n::Message message) {
  char buffer[20];
  Poincare::Print::CustomPrintf(buffer, sizeof(buffer),
                                "%s:", I18n::translate(message));
  textView->setText(buffer);
}

PieBannerView::PieBannerView(Store* store)
    : m_categoryTitle(k_titleFormat),
      m_colorLabel(k_labelFormat),
      m_freqLabel(k_labelFormat),
      m_relativeLabel(k_labelFormat),
      m_colorCell(k_backgroundColor, k_backgroundColor, 1),
      m_freqValue(k_valueFormat),
      m_relativeValue(k_valueFormat),
      m_store(store) {
  FillWithMessageAndColon(&m_colorLabel, I18n::Message::Color);
  FillWithMessageAndColon(&m_freqLabel, I18n::Message::Frequency);
  FillWithMessageAndColon(&m_relativeLabel, I18n::Message::Relative);
}

void PieBannerView::toggleSelection(bool isSelected) {
  if (m_isSelected != isSelected) {
    m_isSelected = isSelected;
    markWholeFrameAsDirty();
  }
}

void PieBannerView::setCategory(int category) {
  if (m_category != category) {
    m_category = category;
    char buffer[sizeof(Store::Label)];
    m_store->getCategoryName(m_category, buffer, sizeof(buffer));
    m_colorCell.setColors(Escher::Palette::DataColorLight[m_category],
                          Escher::Palette::DataColor[m_category]);
    m_categoryTitle.setText(buffer);
    constexpr int bufferSize =
        Poincare::PrintFloat::charSizeForFloatsWithPrecision(k_precision);
    char floatBuffer[bufferSize] = "";
    float p = m_store->getValue(m_group, m_category);
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
        p, floatBuffer, bufferSize, k_precision,
        GlobalPreferences::SharedGlobalPreferences()->displayMode());
    m_freqValue.setText(floatBuffer);
    p = m_store->getRelativeFrequency(m_group, m_category);
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
        p, floatBuffer, bufferSize, k_precision,
        GlobalPreferences::SharedGlobalPreferences()->displayMode());
    m_relativeValue.setText(floatBuffer);
    layoutSubviews(false);
    markWholeFrameAsDirty();
  }
}

void PieBannerView::setGroup(int group) {
  if (m_group != group) {
    m_group = group;
    m_category = k_invalidGroupOrCategory;
    markWholeFrameAsDirty();
  }
}

void PieBannerView::layoutSubviews(bool force) {
  KDCoordinate height = 0;
  KDRect titleRect = KDRect(0, 0, bounds().width(), k_titleCellWidth);
  setChildFrame(&m_categoryTitle, titleRect, force);
  height += titleRect.height();

  // Color label and cell
  KDRect colorTextRect =
      KDRect(k_leftRightMargin, height, bounds().width() - k_leftRightMargin,
             k_textHeight);
  setChildFrame(&m_colorLabel, colorTextRect, force);
  KDRect colorCellRect =
      KDRect(bounds().width() - k_leftRightMargin - k_colorCellWidth, height,
             k_colorCellWidth, k_colorCellHeight);
  setChildFrame(&m_colorCell, colorCellRect, force);
  height += colorTextRect.height() + k_topBottomMargin;

  // Frequency and Relative subviews
  struct LabelAndValue {
    Escher::View* label;
    Escher::View* value;
  } subviews[2] = {{.label = &m_freqLabel, .value = &m_freqValue},
                   {.label = &m_relativeLabel, .value = &m_relativeValue}};
  for (LabelAndValue& subview : subviews) {
    KDRect textRect =
        KDRect(k_leftRightMargin, height, bounds().width() - k_leftRightMargin,
               k_textHeight);
    setChildFrame(subview.label, textRect, force);

    KDSize labelSize = subview.label->minimalSizeForOptimalDisplay();
    KDSize valueSize = subview.value->minimalSizeForOptimalDisplay();
    if (labelSize.width() + valueSize.width() + 2 * k_leftRightMargin >
        bounds().width()) {
      height += k_textHeight;
    }
    KDRect valueRect =
        KDRect(bounds().width() - k_leftRightMargin - valueSize.width(), height,
               valueSize.width(), k_textHeight);
    setChildFrame(subview.value, valueRect, force);
    height += textRect.height() + k_topBottomMargin;
  }
}

Escher::View* PieBannerView::subviewAtIndex(int index) {
  Escher::View* views[k_numberOfSubviews] = {
      &m_categoryTitle, &m_colorLabel, &m_freqLabel,    &m_relativeLabel,
      &m_colorCell,     &m_freqValue,  &m_relativeValue};
  assert(0 <= index && index < k_numberOfSubviews);
  return views[index];
}

}  // namespace Statistics::Categorical
