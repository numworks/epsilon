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
  void drawRect(KDContext* ctx, KDRect rect) const override;

  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  void setVisible(bool visible) override;
  void reloadCell() override;
  void setBuiltinAppDescriptor(const Escher::App::Descriptor* appDescriptor);
  void setExternalApp(Ion::ExternalApps::App app);

 private:
  constexpr static KDCoordinate k_iconMargin = 22;
  constexpr static KDCoordinate k_iconWidth = 55;
  constexpr static KDCoordinate k_iconHeight = 56;
  constexpr static KDCoordinate k_nameWidthMargin = 4;
  constexpr static KDCoordinate k_nameHeightMargin = 1;
  const Escher::TextView* textView() const;
  Escher::ImageView m_iconView;
  Escher::MessageTextView m_messageNameView;
  Escher::Image m_image;
  Escher::PointerTextView m_pointerNameView;
};

}  // namespace Home

#endif
