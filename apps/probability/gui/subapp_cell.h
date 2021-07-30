#ifndef APPS_PROBABILITY_GUI_SUBAPP_CELL_H
#define APPS_PROBABILITY_GUI_SUBAPP_CELL_H

#include <escher/chevron_view.h>
#include <escher/message_table_cell.h>
#include <escher/message_text_view.h>
#include <escher/table_cell.h>

#include "horizontal_or_vertical_layout.h"
#include "image_cell.h"
#include "highlight_image_cell.h"

namespace Probability {

/* Highlightable view with a message */
class HighlightMessageView : public HighlightCell {
public:
  void setHighlighted(bool highlighted) override {
    HighlightCell::setHighlighted(highlighted);
    m_messageView.setBackgroundColor(highlighted ? Palette::Select : KDColorWhite);
  }
  void setMessage(I18n::Message m) { m_messageView.setMessage(m); }
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int i) override { return &m_messageView; }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_messageView.minimalSizeForOptimalDisplay();
  }
  void layoutSubviews(bool force) override { m_messageView.setFrame(bounds(), force); }
  void setFont(const KDFont * font) { m_messageView.setFont(font); }
  void setTextColor(KDColor color) { m_messageView.setTextColor(color); }

private:
  MessageTextView m_messageView;
};

class TitleAndSubtitleView : public VerticalLayout {
public:
  TitleAndSubtitleView();
  Escher::HighlightCell * subviewAtIndex(int i) override;
  int numberOfSubviews() const override { return 2; }
  void setMessages(I18n::Message title, I18n::Message subTitle);
  HighlightMessageView * titleView() { return &m_title; }
  HighlightMessageView * subTitleView() { return &m_subTitle; }

private:
  HighlightMessageView m_title;
  HighlightMessageView m_subTitle;
};

/* Cell made of an icon, a title, a chevron and a subtitle below. */
class SubappCell : public Escher::TableCell {
public:
  void setHighlighted(bool highlighted) override;
  Escher::View * subviewAtIndex(int i) override;
  int numberOfSubviews() const override { return 3; }
  const View * accessoryView() const override { return &m_chevron; }
  const View * labelView() const override { return &m_icon; }
  const View * subLabelView() const override { return &m_titleAndSubtitleView; }
  bool isSublabelAlignedRight() const override { return false; }

  void setImage(const Escher::Image * image);
  void setMessages(I18n::Message title, I18n::Message subTitle);

private:
  HighlightImageCell m_icon;
  TitleAndSubtitleView m_titleAndSubtitleView;
  ChevronView m_chevron;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_SUBAPP_CELL_H */
