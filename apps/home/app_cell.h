#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher/app.h>
#include <escher/highlight_cell.h>
#include <escher/image_view.h>
#include <escher/pointer_text_view.h>
#include <ion/external_apps.h>

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
  void setExternalApp(Ion::ExternalApps::App app);
private:
  static constexpr KDCoordinate k_iconMargin = 22;
  static constexpr KDCoordinate k_iconWidth = 55;
  static constexpr KDCoordinate k_iconHeight = 56;
  static constexpr KDCoordinate k_nameWidthMargin = 4;
  static constexpr KDCoordinate k_nameHeightMargin = 1;
  const Escher::TextView * textView() const;
  Escher::ImageView m_iconView;
  Escher::MessageTextView m_messageNameView;
  Escher::PointerTextView m_pointerNameView;
  bool m_visible;
};

}

#endif
