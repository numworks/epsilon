#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "cursor_view.h"

#define GRAPH_VIEW_IS_TILED 1

class GraphView : public
#if GRAPH_VIEW_IS_TILED
                  TiledView
#else
                  View
#endif
{
public:
  GraphView();

#if GRAPH_VIEW_IS_TILED
  KDColor * tile() const override;
  KDSize tileSize() const override;
  void drawTile(KDContext * ctx, KDRect rect) const override;
#else
  void drawRect(KDContext * ctx, KDRect rect) const override;
#endif

//  void drawRect(KDRect rect) const override;
  void moveCursorRight();
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };

  float min(Axis axis) const;
  float max(Axis axis) const;
  KDCoordinate pixelLength(Axis axis) const;

  float pixelToFloat(Axis axis, KDCoordinate p) const;
  KDCoordinate floatToPixel(Axis axis, float f) const;

  void drawLine(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, KDColor color, KDCoordinate thickness = 1) const;

  void drawAxes(KDContext * ctx, KDRect rect) const;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, int count, KDColor color) const;
  void drawFunction(KDContext * ctx, KDRect rect) const;

  static constexpr KDCoordinate kTileWidth = 32;
  static constexpr KDCoordinate kTileHeight = 32;
  KDColor m_tile[kTileWidth*kTileHeight];

  CursorView m_cursorView;
  KDPoint m_cursorPosition;

  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
};

#endif
