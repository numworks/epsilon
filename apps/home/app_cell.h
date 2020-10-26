#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher/app.h>
#include <escher/highlight_cell.h>
#include <escher/image_view.h>

namespace Home {

class AppCell : public Escher::HighlightCell {
public:
  AppCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;

  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  void setVisible(bool visible);
  void reloadCell() override;
  void setAppDescriptor(Escher::App::Descriptor * appDescriptor);
private:
  static constexpr KDCoordinate k_iconMargin = 22;
  static constexpr KDCoordinate k_iconWidth = 55;
  static constexpr KDCoordinate k_iconHeight = 56;
  static constexpr KDCoordinate k_nameWidthMargin = 4;
  static constexpr KDCoordinate k_nameHeightMargin = 1;
  Escher::ImageView m_iconView;
  Escher::MessageTextView m_nameView;
  bool m_visible;
};

}

#endif
