#ifndef ESCHER_SUBAPP_CELL_H
#define ESCHER_SUBAPP_CELL_H

#include <escher/bordered.h>
#include <escher/chevron_view.h>
#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>
#include <escher/transparent_image_view.h>
namespace Escher {

/* Cell made of an icon, a title, a chevron and a subtitle below. */
class SubappCell : public HighlightCell, public Bordered {
public:
  SubappCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlighted(bool highlighted) override;
  View * subviewAtIndex(int i) override;
  int numberOfSubviews() const override { return 4; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;

  void setImage(const Image * image);
  void setMessages(I18n::Message title, I18n::Message subTitle);

private:
  constexpr static int k_verticalMarginTop = 10;
  constexpr static int k_verticalMarginBetweenTexts = 5;
  constexpr static int k_verticalMarginBottom = 7;
  constexpr static int k_horizontalMarginBetweenTextAndIcon = 15;

  TransparentImageView m_icon;
  MessageTextView m_title;
  MessageTextView m_subTitle;
  ChevronView m_chevron;
};

}

#endif
