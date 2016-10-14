#ifndef ESCHER_IMAGE_VIEW_H
#define ESCHER_IMAGE_VIEW_H

#include <escher/view.h>
#include <escher/image.h>

class ImageView : public View {
public:
  ImageView();
  void setImage(const Image * image);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  const Image * m_image;
};

#endif
