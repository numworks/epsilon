#ifndef ESCHER_TILED_VIEW_H
#define ESCHER_TILED_VIEW_H

#include <escher/view.h>

class TiledView : public View {
  using View::View;
protected:
  void drawRect(KDContext * ctx, KDRect rect) const override;
  virtual void drawTile(KDContext * ctx, KDRect rect) const = 0;

  virtual KDColor * tile() const = 0;
  virtual KDSize tileSize() const = 0;
};

#endif
