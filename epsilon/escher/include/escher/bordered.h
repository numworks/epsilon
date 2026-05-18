#pragma once

#include <escher/metric.h>
#include <escher/theme/components.h>
#include <kandinsky/context.h>

namespace Escher {

class Bordered {
 public:
  void drawBorderOfRect(KDContext* ctx, KDRect rect,
                        ThemeComponent borderColor) const;
  void drawTopBorderOfRect(KDContext* ctx, KDRect rect,
                           ThemeComponent borderColor) const;
  void drawInnerRect(KDContext* ctx, KDRect rect,
                     ThemeComponent backgroundColor) const;

  // Mask of borders to draw
  struct Mask {
    bool top : 1;
    bool right : 1;
    bool bottom : 1;
    bool left : 1;
  };
  constexpr static Mask k_fullBorder = {
      .top = true, .right = true, .bottom = true, .left = true};
  // Draw borders within the rect
  static void DrawInnerBorderOfRect(KDContext* ctx, KDRect rect,
                                    KDCoordinate size, KDColor color,
                                    Mask mask = k_fullBorder);
  // Draw borders outside of the rect
  static void DrawOuterBorderOfRect(KDContext* ctx, KDRect rect,
                                    KDCoordinate size, KDColor color,
                                    Mask mask = k_fullBorder);

 protected:
  constexpr static KDCoordinate k_separatorThickness =
      Metric::CellSeparatorThickness;

 private:
  /* If drawAllCorners is false, corners adjacent to a single border won't be
   * drawn. For example:
   * drawAllCorners | Top and Right borders | All borders |
   *                |        █████          |    █████    |
   *    true        |        ░   █          |    █   █    |
   *                |        ░░░░█          |    █████    |
   *                |                       |             |
   *                |        ░████          |    █████    |
   *    false       |        ░   █          |    █   █    |
   *                |        ░░░░░          |    █████    |
   */
  static void PrivateDrawInnerBorderOfRect(KDContext* ctx, KDRect rect,
                                           KDCoordinate size, KDColor color,
                                           Mask mask, bool drawAllCorners);
};

}  // namespace Escher
