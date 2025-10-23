
#include "pie_banner_view.h"

namespace Statistics::Categorical {

PieBannerView::PieBannerView(Store* store)
    : m_categoryTitle({.style = {.backgroundColor = k_backgroundColor},
                       .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_colorLabel(labelFormat),
      m_freqLabel(labelFormat),
      m_relativeLabel(labelFormat),
      m_colorCell(k_backgroundColor, k_backgroundColor, 1),
      m_freqValue(valueFormat),
      m_relativeValue(valueFormat),
      m_store(store) {
  fillWithMessageAndColon(&m_colorLabel, I18n::Message::Color);
  fillWithMessageAndColon(&m_freqLabel, I18n::Message::Frequency);
  fillWithMessageAndColon(&m_relativeLabel, I18n::Message::Relative);
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

void PieBannerView::fillWithMessageAndColon(Escher::TextView* textView,
                                            I18n::Message message) {
  char buffer[20];
  Poincare::Print::CustomPrintf(buffer, sizeof(buffer),
                                "%s:", I18n::translate(message));
  textView->setText(buffer);
}

void PieBannerView::layoutSubviews(bool force) {
  KDCoordinate height = 0;
  KDRect titleRect = KDRect(0, 0, bounds().width(), 40);
  setChildFrame(&m_categoryTitle, titleRect, force);
  height += titleRect.height();

  constexpr KDCoordinate k_verticalMargin = 15;
  constexpr KDCoordinate k_horizontalMargin = 5;

  // Color label and cell
  constexpr KDCoordinate k_colorCellWidth = 40;
  constexpr KDCoordinate k_textHeigth = 20;
  constexpr KDCoordinate k_colorCellHeigth = 15;
  KDRect colorTextRect =
      KDRect(k_verticalMargin, height, bounds().width() - k_verticalMargin,
             k_textHeigth);
  setChildFrame(&m_colorLabel, colorTextRect, force);
  KDRect colorCellRect =
      KDRect(bounds().width() - k_verticalMargin - k_colorCellWidth, height,
             k_colorCellWidth, k_colorCellHeigth);
  setChildFrame(&m_colorCell, colorCellRect, force);
  height += colorTextRect.height() + k_horizontalMargin;

  // Frequency label
  KDRect freqTextRect =
      KDRect(k_verticalMargin, height, bounds().width() - k_verticalMargin,
             k_textHeigth);
  setChildFrame(&m_freqLabel, freqTextRect, force);

  // Frequency value
  KDSize labelSize = m_freqLabel.minimalSizeForOptimalDisplay();
  KDSize valueSize = m_freqValue.minimalSizeForOptimalDisplay();
  if (labelSize.width() + valueSize.width() + 2 * k_verticalMargin >
      bounds().width()) {
    height += k_textHeigth;
  }
  KDRect freqValueRect =
      KDRect(bounds().width() - k_verticalMargin - valueSize.width(), height,
             valueSize.width(), k_textHeigth);
  setChildFrame(&m_freqValue, freqValueRect, force);
  height += freqTextRect.height() + k_horizontalMargin;

  // Relative label and value
  KDRect relativeTextRect =
      KDRect(k_verticalMargin, height, bounds().width() - k_verticalMargin,
             k_textHeigth);
  setChildFrame(&m_relativeLabel, relativeTextRect, force);

  // Relative value
  labelSize = m_relativeLabel.minimalSizeForOptimalDisplay();
  valueSize = m_relativeValue.minimalSizeForOptimalDisplay();
  if (labelSize.width() + valueSize.width() + 2 * k_verticalMargin >
      bounds().width()) {
    height += k_textHeigth;
  }
  KDRect relativeValueRect =
      KDRect(bounds().width() - k_verticalMargin - valueSize.width(), height,
             valueSize.width(), k_textHeigth);
  setChildFrame(&m_relativeValue, relativeValueRect, force);
}

Escher::View* PieBannerView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_categoryTitle;
    case 1:
      return &m_colorLabel;
    case 2:
      return &m_freqLabel;
    case 3:
      return &m_relativeLabel;
    case 4:
      return &m_colorCell;
    case 5:
      return &m_freqValue;
    case 6:
      return &m_relativeValue;
  }
  OMG::unreachable();
}

}  // namespace Statistics::Categorical
